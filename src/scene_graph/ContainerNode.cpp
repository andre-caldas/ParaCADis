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

#include <cassert>
#include <iostream>

namespace SceneGraph
{

  SharedPtr<ContainerNode>
  ContainerNode::create_document(const SharedPtr<SceneRoot>& scene_root,
                                 const SharedPtr<document_t>& document)
  {
    std::shared_ptr<ContainerNode> result{new ContainerNode(scene_root)};
    result->self = result;
    result->populate(document);
    return result;
  }

  SharedPtr<ContainerNode>
  ContainerNode::create(const SharedPtr<ContainerNode>& parent,
                        const SharedPtr<container_t>& self_container)
  {
    auto scene_root = parent->sceneRootWeak.lock();
    std::shared_ptr<ContainerNode> result{new ContainerNode(scene_root, parent)};
    result->self = result;
    result->populate(self_container);
    return result;
  }


  ContainerNode::ContainerNode(const SharedPtr<SceneRoot>& scene_root)
      : sceneRootWeak(scene_root)
  {
    ogreNode = scene_root->sceneManager->createSceneNode(Ogre::SCENE_STATIC);
    auto* root_node = scene_root->sceneManager->getRootSceneNode();
    root_node->addChild(ogreNode);
  }

  ContainerNode::ContainerNode(const SharedPtr<SceneRoot>& scene_root,
                               const SharedPtr<ContainerNode>& parent)
      : sceneRootWeak(scene_root)
  {
    assert(parent);
    ogreNode = scene_root->sceneManager->createSceneNode(Ogre::SCENE_STATIC);
    auto* parent_ogre_node = parent->ogreNode;
    parent_ogre_node->addChild(ogreNode);
  }

  void ContainerNode::populate(const SharedPtr<container_t>& my_container)
  {
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
      addContainer(std::move(child));
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
          my_container, queue, self_lock, &ContainerNode::addMesh);
      my_container->remove_non_container_sig.connect(
          my_container, queue, self_lock, &ContainerNode::removeMesh);
      my_container->move_non_container_sig.connect(
          my_container, queue, self_lock, &ContainerNode::moveMesh);

      // Block the queue before releasing the lock.
      queue->block(this);
    }

    // These might take some time, because it generates NURBS.
    for(auto&& mesh: new_non_containers) {
      addMesh(std::move(mesh));
    }
    queue->unblock(this);
  }

  void ContainerNode::addContainer(SharedPtr<container_t> added_container)
  {
    auto self_lock = self.lock();
    ContainerNode::create(std::move(self_lock), std::move(added_container));
  }

  void ContainerNode::removeContainer(SharedPtr<container_t> removed_container)
  {
    assert(false && "Implement removeContainer!");
  }

  void ContainerNode::moveContainer(SharedPtr<container_t> moved_container,
                                    SharedPtr<container_t> moved_to)
  {
    assert(false && "Implement moveContainer!");
  }

  void ContainerNode::addMesh(SharedPtr<exporter_t> mesh)
  {
    assert(false && "Implement!");
  }

  void ContainerNode::removeMesh(SharedPtr<exporter_t> mesh)
  {
    assert(false && "Implement removeMesh!");
  }

  void ContainerNode::moveMesh(SharedPtr<exporter_t> mesh,
                                SharedPtr<container_t> moved_to)
  {
    assert(false && "Implement moveMesh!");
  }
}
