// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2024 André Caldas <andre.em.caldas@gmail.com>            *
 *                                                                          *
 *   This file is part of ParaCADis.                                        *
 *                                                                          *
 *   ParaCADis is free software: you can redistribute it and/or modify it   *
 *   under the terms of the GNU General Public License as published         *
 *   by the Free Software Foundation, either version 2.1 of the License,    *
 *   or (at your option) any later version.                                 *
 *                                                                          *
 *   ParaCADis is distributed in the hope that it will be useful, but       *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of             *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                   *
 *   See the GNU General Public License for more details.                   *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with ParaCADis. If not, see <https://www.gnu.org/licenses/>.     *
 *                                                                          *
 ***************************************************************************/

#include "ContainerNode.h"

#include "SceneRoot.h"
#include "MeshNode.h"

#include <base/expected_behaviour/CycleGuard.h>

#include <cassert>
#include <iostream>

#include <OGRE/OgreSceneManager.h>

namespace SceneGraph
{

  SharedPtr<ContainerNode>
  ContainerNode::create_root_node(const SharedPtr<SceneRoot>& scene_root,
                                  const SharedPtr<document_t>& document)
  {
    std::shared_ptr<ContainerNode> result(new ContainerNode{scene_root});
    result->self = result;
    CycleGuard<container_t> cycle_guard;
    result->populate(cycle_guard, document);
    return result;
  }

  SharedPtr<ContainerNode>
  ContainerNode::create(CycleGuard<container_t>& cycle_guard,
                        const SharedPtr<ContainerNode>& parent,
                        const SharedPtr<container_t>& self_container)
  {
    assert(static_cast<document_t*>(self_container.get()) == nullptr &&
           "Use create_root_node() to associate a document to a node.");
    auto scene_root = parent->sceneRootWeak.lock();
    std::shared_ptr<ContainerNode> sptr{new ContainerNode(scene_root, parent)};
    auto result = SharedPtr{std::move(sptr)};
    result->self = result;
    result->populate(cycle_guard, self_container);
    return result;
  }


  ContainerNode::ContainerNode(const SharedPtr<SceneRoot>& scene_root)
      : sceneRootWeak(scene_root)
  {
    auto* ogreNode = scene_root->sceneManager->createSceneNode();
    ogreNodeWeak = SharedPtr{scene_root, ogreNode};
    auto* root_node = scene_root->sceneManager->getRootSceneNode();
    root_node->addChild(ogreNode);
  }

  ContainerNode::ContainerNode(const SharedPtr<SceneRoot>& scene_root,
                               const SharedPtr<ContainerNode>& parent)
      : sceneRootWeak(scene_root)
  {
    assert(parent);
    auto* ogreNode = scene_root->sceneManager->createSceneNode();
    ogreNodeWeak = SharedPtr{scene_root, ogreNode};
    auto parent_ogre_node = parent->ogreNodeWeak.lock();
    assert(parent_ogre_node && "Parent OGRE SceneNode was supposed to be alive.");
    parent_ogre_node->addChild(ogreNode);
  }

  void ContainerNode::populate(CycleGuard<container_t>& cycle_guard,
                               const SharedPtr<container_t>& my_container)
  {
    auto sentinel = cycle_guard.sentinel(my_container.get());
    if(!sentinel.success()) {
      std::cerr << "Cycle detected!";
      return;
    }

    auto scene_root = sceneRootWeak.lock();
    if(!scene_root) { return; }

    auto self_lock = self.lock();
    assert(self_lock);
    assert(self_lock.get() == this && "Pointer 'self' must be myself!");
    auto& queue = scene_root->getQueue();
    assert(queue);
    if(!queue) { return; }

    std::vector<SharedPtr<container_t>> new_containers;
    { // Lock
      // The containers in my_container will not change till the end of this block.
      // Suposedly, no messagens relative to those containers will be queued.
      auto view_lock = my_container->containersView();
      new_containers.reserve(view_lock.size());
      for(auto& [k,child]: view_lock) {
        new_containers.push_back(child);
      }

      // It is very important that the signals are emited while
      // the emiter still holds the lock. Otherwise, we might
      // miss some signal between populating and connecting.
      my_container->add_container_sig.connect(
          my_container, queue, self_lock, &ContainerNode::addContainer);
      my_container->remove_container_sig.connect(
          my_container, queue, self_lock, &ContainerNode::removeContainer);
      my_container->move_container_sig.connect(
          my_container, queue, self_lock, &ContainerNode::moveContainer);

      // Block the queue before releasing the lock.
      queue->block(this);
    }

    // This might take long (recursive!),
    // so we do not hold any locks while processing it.
    for(auto&& child: new_containers) {
      addContainerCycleGuard(cycle_guard, std::move(child));
    }
    queue->unblock(this);

    std::vector<SharedPtr<NamingScheme::ExporterBase>> new_non_containers;
    { // Lock
      auto view_lock = my_container->nonContainersView();
      new_non_containers.reserve(view_lock.size());
      for(auto& [k,leaf]: view_lock) {
        new_non_containers.push_back(leaf);
      }

      // It is very important that the signals are emited while
      // the emiter still holds the lock. Otherwise, we might
      // miss some signal between populating and connecting.
      my_container->add_non_container_sig.connect(
          my_container, queue, self_lock, &ContainerNode::addNonContainer);
      my_container->remove_non_container_sig.connect(
          my_container, queue, self_lock, &ContainerNode::removeNonContainer);
      my_container->move_non_container_sig.connect(
          my_container, queue, self_lock, &ContainerNode::moveNonContainer);

      // Block the queue before releasing the lock.
      queue->block(this);
    }

    // These might take some time, because it generates geometry meshes.
    for(auto&& non_container: new_non_containers) {
      addNonContainer(std::move(non_container));
    }
    queue->unblock(this);
  }

  void ContainerNode::addContainerCycleGuard(CycleGuard<container_t>& cycle_guard,
                                             SharedPtr<container_t> added_container)
  {
    auto self_lock = self.lock();
    auto new_node = ContainerNode::create(
        cycle_guard, std::move(self_lock), added_container);

    auto scene_root = sceneRootWeak.lock();
    if(!scene_root) {
      return;
    }
    Threads::WriterGate gate{scene_root->containerNodes};
    gate->emplace(added_container.get(), std::move(new_node));
  }

  void ContainerNode::addContainer(SharedPtr<container_t> added_container)
  {
    CycleGuard<container_t> cycle_guard;
    addContainerCycleGuard(cycle_guard, added_container);
  }

  void ContainerNode::removeContainer(SharedPtr<container_t> removed_container)
  {
    auto scene_root = sceneRootWeak.lock();
    if(!scene_root) {
      return;
    }
    Threads::WriterGate gate{scene_root->containerNodes};
    gate->extract(removed_container.get());
  }

  void ContainerNode::moveContainer(SharedPtr<container_t> moved_container,
                                    SharedPtr<container_t> moved_to)
  {
    assert(false && "Implement!");
  }

  void ContainerNode::addNonContainer(SharedPtr<non_container_t> non_container)
  {
    assert(non_container && "SharedPtr is not supposed to be empty");
    auto geo = non_container.cast_nothrow<geometry_t>();
    if(geo) {
      addMesh(std::move(geo));
      return;
    }
  }

  void ContainerNode::removeNonContainer(SharedPtr<non_container_t> non_container)
  {
    assert(non_container && "SharedPtr is not supposed to be empty");
    auto geo = non_container.cast_nothrow<geometry_t>();
    if(geo) {
      removeMesh(std::move(geo));
      return;
    }
  }

  void ContainerNode::moveNonContainer(SharedPtr<non_container_t> non_container,
                                       SharedPtr<container_t> moved_to)
  {
    assert(false && "Implement moveNonContainer!");
  }


  void ContainerNode::addMesh(SharedPtr<geometry_t> geo)
  {
    auto scene_root = sceneRootWeak.lock();
    if(!scene_root) {
      return;
    }

    SharedPtr<MeshNode> new_mesh_node;
    {
      Threads::ReaderGate gate{scene_root->meshNodes};
      auto it = gate->find(geo.get());
      if(it != gate->end()) {
        new_mesh_node = it->second;
        assert(new_mesh_node && "Mapped MeshNode is supposed to be valid.");
      }
    }

    if(!new_mesh_node) {
assert(false);
//      xxxxxxx create a mesh_provider;
//      new_mesh_node = MeshNode::make_shared(mesh_provider);
    }

    Threads::WriterGate gate{scene_root->meshNodes};
    gate->emplace(geo.get(), std::move(new_mesh_node));
  }

  void ContainerNode::removeMesh(SharedPtr<geometry_t> geo)
  {
    auto scene_root = sceneRootWeak.lock();
    if(!scene_root) {
      return;
    }

    Threads::WriterGate gate{scene_root->meshNodes};
    auto nh = gate->extract(geo.get());
    assert(nh && "Nothing extracted.ß");
  }
}
