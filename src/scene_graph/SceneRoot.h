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

#include <base/document_tree/Container.h>
#include <base/document_tree/DocumentTree.h>
#include <base/threads/message_queue/SignalQueue.h>
#include <base/threads/safe_structs/ThreadSafeMap.h>

#include <memory>

namespace Ogre {
  class SceneManager;
}

namespace SceneGraph
{
  /**
   * The SceneRoot is the OGRE side of the DocumentTree.
   *
   * A DocumentTree is associated to a SceneRoot.
   * Every Container is associated to a ContainerNode and
   * every geometry (Exporter) is associated to a NurbsNode.
   */
  class SceneRoot
  {
    friend class ContainerNode;

  public:
    SceneRoot(Ogre::SceneManager& scene_manager);

    /**
     * Initiates the bridge structure between the document and
     * the OGRE scene graph.
     */
    static void populate(const SharedPtr<SceneRoot>& self,
                         const SharedPtr<Document::DocumentTree>& document);

    void runQueue();

    const SharedPtr<Threads::SignalQueue>& getQueue() { return signalQueue; }

  private:
    WeakPtr<SceneRoot>              self;
    SharedPtr<Threads::SignalQueue> signalQueue;

    SharedPtr<ContainerNode> rootContainer;

    /**
     * The document tree structure is kept flat.
     * We do not use a tree here.
     *
     * The document tree strucutre is transported directly to the OGRE scene.
     */
    /// @{
    template<typename Key, typename Val>
    using map_t = Threads::SafeStructs::ThreadSafeUnorderedMultimap<Key, Val>;
    map_t<container_t*, SharedPtr<ContainerNode>> containerNodes;
    map_t<geometry_t*,  SharedPtr<MeshNode>>      meshNodes;
    /// @}

  /* OGRE stuff */
  public:
    Ogre::SceneManager* sceneManager = nullptr;
  };
}
