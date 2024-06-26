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

#include <base/document_tree/DocumentTree.h>

#include <OGRE/OgreSceneNode.h>

#include <memory>

namespace SceneGraph
{

  using container_t = Document::Container;
  using document_t = Document::DocumentTree;
  using exporter_t = NamingScheme::ExporterBase;

  class SceneRoot;

  /**
   * Non-leaf scene graph node.
   */
  class ContainerNode
  {
  public:
    virtual ~ContainerNode() = default;

    static SharedPtr<ContainerNode>
    create_document(const SharedPtr<SceneRoot>& scene_root,
                    const SharedPtr<document_t>& document);

    static SharedPtr<ContainerNode>
    create(const SharedPtr<ContainerNode>& parent,
           const SharedPtr<container_t>& self_container);

    void addContainer(SharedPtr<container_t> added_container);
    void removeContainer(SharedPtr<container_t> removed_container);
    void moveContainer(SharedPtr<container_t> moved_container,
                       SharedPtr<container_t> moved_to);

    void addMesh(SharedPtr<exporter_t> mesh);
    void removeMesh(SharedPtr<exporter_t> mesh);
    void moveMesh(SharedPtr<exporter_t> mesh,
                   SharedPtr<container_t> moved_to);

  protected:
    ContainerNode(const SharedPtr<SceneRoot>& scene_root);
    ContainerNode(const SharedPtr<SceneRoot>& scene_root,
                  const SharedPtr<ContainerNode>& parent);

    void populate(const SharedPtr<container_t>& my_container);

  private:
    WeakPtr<SceneRoot>     sceneRootWeak;
    WeakPtr<ContainerNode> self;
    Ogre::SceneNode*       ogreNode = nullptr;
  };

}
