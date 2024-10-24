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
#include <mesh_provider/MeshProvider.h>

#include <cassert>
#include <exception>
#include <iostream>

#include <OGRE/OgreEntity.h>
#include <OGRE/OgreSceneManager.h>

namespace SceneGraph
{

  SharedPtr<ContainerNode>
  ContainerNode::create_root_node(const SharedPtr<SceneRoot>& scene_root,
                                  const SharedPtr<document_t>& document)
  {
    std::shared_ptr<ContainerNode> result(new ContainerNode{scene_root});
    result->self = result;

    auto* root_node = scene_root->sceneManager->getRootSceneNode();
    result->ogreNodeWeak = SharedPtr{scene_root, root_node};

    CycleGuard<container_t> cycle_guard;
    result->populate(cycle_guard, document);
    return result;
  }


  ContainerNode::ContainerNode(const SharedPtr<SceneRoot>& scene_root)
      : sceneRootWeak(scene_root)
  {
    auto* ogre_node = scene_root->sceneManager->createSceneNode();
    ogreNodeWeak = SharedPtr{scene_root, ogre_node};
  }

  ContainerNode::~ContainerNode()
  {
    auto ogre_node = ogreNodeWeak.lock();
    assert(ogre_node);
    auto scene_root = sceneRootWeak.lock();
    assert(scene_root);
    scene_root->sceneManager->destroySceneNode(ogre_node.get());
  }


  void ContainerNode::populate(CycleGuard<container_t>& cycle_guard,
                               const SharedPtr<container_t>& my_container)
  {
    auto cycle_sentinel = cycle_guard.sentinel(my_container.get());
    if(!cycle_sentinel.success()) {
      // TODO: remove output. Not a bug.
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
    auto scene_root = sceneRootWeak.lock();
    if(!scene_root) {
      return;
    }

    auto new_node = SharedPtr<ContainerNode>::from_pointer(new ContainerNode(scene_root));
    new_node->self = new_node;

    { // Scoped lock.
      Threads::WriterGate gate{containerNodes};
      auto [it, success] = gate->emplace(added_container.get(), new_node);
      if(!success) {
        throw std::runtime_error("Container alredy a child of this node");
      }
    }

    new_node->populate(cycle_guard, added_container);

    auto ogre_node = ogreNodeWeak.lock();
    auto ogre_new_node = new_node->ogreNodeWeak.lock();
    assert(ogre_node);
    assert(ogre_new_node);
    ogre_node->addChild(ogre_new_node.get());
  }

  void ContainerNode::addContainer(SharedPtr<container_t> added_container)
  {
    CycleGuard<container_t> cycle_guard;
    addContainerCycleGuard(cycle_guard, std::move(added_container));
  }

  void ContainerNode::removeContainer(SharedPtr<container_t> removed_container)
  {
    auto scene_root = sceneRootWeak.lock();
    if(!scene_root) {
      return;
    }

    auto ogre_node = ogreNodeWeak.lock();
    assert(ogre_node);
    Ogre::SceneNode* ogre_removed_node = nullptr;

    SharedPtr<ContainerNode> removed_node;
    { // Scoped lock.
      Threads::WriterGate gate{containerNodes};
      auto nh = gate->extract(removed_container.get());
      if(!nh) {
        throw std::runtime_error("Not a child of this node");
      }
      removed_node = std::move(nh.mapped());
    }

    assert(removed_node);
    auto ogre_removed_lock = removed_node->ogreNodeWeak.lock();
    assert(ogre_removed_lock);
    ogre_removed_node = ogre_removed_lock.get();

    assert(ogre_removed_node);
    ogre_node->removeChild(ogre_removed_node);
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

    // Prepare stuff before holding a lock.
    auto scene = sceneRootWeak.lock();
    if(!scene) {
      return;
    }
    auto mesh_provider = Mesh::MeshProvider::make_shared(geo, scene->getQueue());
    auto temp_mesh_node = MeshNode::make_shared(std::move(mesh_provider));

    { // Scoped lock.
      Threads::WriterGate gate{scene_root->meshNodes};
      if(!new_mesh_node) {
        // We check again because we have released the lock.
        auto it = gate->find(geo.get());
        if(it != gate->end()) {
          new_mesh_node = it->second;
          assert(new_mesh_node && "Mapped MeshNode is supposed to be valid.");
        } else {
          new_mesh_node = std::move(temp_mesh_node);
        }
      }
      gate->emplace(geo.get(), new_mesh_node);
    }

    auto mesh = new_mesh_node->getOgreMesh();
    auto mesh_entity = scene_root->sceneManager->createEntity(mesh.sliced());
    mesh_entity->setMaterialName("WoodPallet");

    auto ogre_node = ogreNodeWeak.lock();
    assert(ogre_node);
    ogre_node->attachObject(mesh_entity);
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
