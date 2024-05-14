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
    std::shared_ptr<ContainerNode> result{new ContainerNode(std::move(scene_root))};
    result->self = result;
    result->populate(self_container);
    return result;
  }


  ContainerNode::ContainerNode(const SharedPtr<SceneRoot>& scene_root)
      : sceneRootWeak(scene_root)
  {
    ogreNode = scene_root->sceneManager->createSceneNode(Ogre::SCENE_STATIC);
    auto* parent = scene_root->sceneManager->getRootSceneNode();
    parent->addChild(ogreNode);
  }

  void ContainerNode::populate(const SharedPtr<container_t>& my_container)
  {
    auto scene_root = sceneRootWeak.lock();
    if(!scene_root) { return; }

    auto self_lock = self.lock();
    assert(self_lock);
    auto& queue = scene_root->getQueue();

    { // Lock
      auto view_lock = my_container->containersView();
      for(auto& [k,child]: view_lock) {
        addContainer(child);
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
    }

    { // Lock
      auto view_lock = my_container->nonContainersView();
      for(auto& [k,nurbs]: view_lock) {
        addNurbs(nurbs);
      }

      // It is very important that the signals are emited while
      // the emiter still holds the lock. Otherwise, we might
      // miss some signal between populating and connecting.
      my_container->add_non_container_sig.connect(
          my_container, queue, self_lock, &ContainerNode::addNurbs);
      my_container->remove_non_container_sig.connect(
          my_container, queue, self_lock, &ContainerNode::removeNurbs);
      my_container->move_non_container_sig.connect(
          my_container, queue, self_lock, &ContainerNode::moveNurbs);
    }
  }

  void ContainerNode::addContainer(SharedPtr<container_t> added_container)
  {
    assert(false && "Implement!");
  }

  void ContainerNode::removeContainer(SharedPtr<container_t> removed_container)
  {
    assert(false && "Implement!");
  }

  void ContainerNode::moveContainer(SharedPtr<container_t> moved_container,
                                    SharedPtr<container_t> moved_to)
  {
    assert(false && "Implement!");
  }

  void ContainerNode::addNurbs(SharedPtr<exporter_t> nurbs)
  {
    assert(false && "Implement!");
  }

  void ContainerNode::removeNurbs(SharedPtr<exporter_t> nurbs)
  {
    assert(false && "Implement!");
  }

  void ContainerNode::moveNurbs(SharedPtr<exporter_t> nurbs,
                                SharedPtr<container_t> moved_to)
  {
    assert(false && "Implement!");
  }
}
