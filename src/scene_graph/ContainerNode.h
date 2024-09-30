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

#pragma once

#include "forwards.h"

#include <memory>

#include <base/expected_behaviour/CycleGuard.h>
#include <base/expected_behaviour/SharedPtr.h>

namespace SceneGraph
{
  /**
   * Non-leaf scene graph node.
   */
  class ContainerNode
  {
  public:
    virtual ~ContainerNode() = default;

    /**
     * Creates a new "root" ContainerNode,
     * to be attached to a nodeless SceneRoot.
     */
    static SharedPtr<ContainerNode>
    create_root_node(const SharedPtr<SceneRoot>& scene_root,
                     const SharedPtr<document_t>& document);

    /**
     * Creates a new ContainerNode, to be attached to @a parent.
     *
     * @param parent - Parent ContainerNode.
     * @param self_container - the actual Document::Container this node represents.
     */
    static SharedPtr<ContainerNode>
    create(CycleGuard<container_t>& cycle_guard,
           const SharedPtr<ContainerNode>& parent,
           const SharedPtr<container_t>& self_container);

    void addContainerCycleGuard(CycleGuard<container_t>& cycle_guard,
                                SharedPtr<container_t> added_container);
    void addContainer(SharedPtr<container_t> added_container);
    void removeContainer(SharedPtr<container_t> removed_container);
    void moveContainer(SharedPtr<container_t> moved_container,
                       SharedPtr<container_t> moved_to);

    void addNonContainer(SharedPtr<non_container_t> geo);
    void removeNonContainer(SharedPtr<non_container_t> geo);
    void moveNonContainer(SharedPtr<non_container_t> geo,
                          SharedPtr<container_t> moved_to);

    void addMesh(SharedPtr<geometry_t> geo);
    void removeMesh(SharedPtr<geometry_t> geo);

  protected:
    ContainerNode(const SharedPtr<SceneRoot>& scene_root);
    ContainerNode(const SharedPtr<SceneRoot>& scene_root,
                  const SharedPtr<ContainerNode>& parent);

    void populate(CycleGuard<container_t>& cycle_guard,
                  const SharedPtr<container_t>& my_container);

  private:
    /**
     * We need to lock sceneRootWeak in order to manipulate OGRE's scene.
     * As long as SceneRoot is alive, OGRE's scene is also alive.
     */
    WeakPtr<SceneRoot> sceneRootWeak;
    /**
     * We do not need to lock a shared pointer to ourselves.
     * This is only used so we can pass a shared pointer to ourselves to a function.
     */
    WeakPtr<ContainerNode> self;
    /**
     * Pointer to the OGRE node associated to the same control block as sceneRootWeak.
     *
     * Notice that OGRE destroys the nodes when the scene is destroyed.
     * We do not "delete" an ogre node ourselves.
     *
     * @attention
     * The OGRE node is not to be created using std::make_shared.
     * It is supposed to be constructed like this:
     * auto sceneRoot = sceneRootWeak;
     * if(!sceneRoot) return;
     * auto* ogreNode = scene_root->sceneManager->createSceneNode();
     * ogreNodeWeak = SharedPtr{sceneRoot, ogreNode};
     */
    WeakPtr<Ogre::SceneNode> ogreNodeWeak;
  };
}
