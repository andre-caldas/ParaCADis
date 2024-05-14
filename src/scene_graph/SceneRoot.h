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

#include "ContainerNode.h"

#include <base/document_tree/Container.h>
#include <base/document_tree/DocumentTree.h>
#include <base/threads/message_queue/SignalQueue.h>

#include <memory>
#include <OGRE/OgreRoot.h>

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
  public:
    SceneRoot();

    /**
     * Initiates the bridge structure between the document and
     * the OGRE scene graph.
     */
    void populate(const SharedPtr<SceneRoot>& self,
                  const SharedPtr<DocumentTree::DocumentTree>& document);

    void runQueue();

    Ogre::Root& getOgreRoot() { return *ogreRoot; }
    const SharedPtr<Threads::SignalQueue>& getQueue() { return signalQueue; }

  private:
    WeakPtr<SceneRoot>              self;
    SharedPtr<Threads::SignalQueue> signalQueue;

    std::unique_ptr<Ogre::Root> ogreRoot;

    SharedPtr<ContainerNode> rootContainer;


  public:
    Ogre::SceneManager*        sceneManager  = nullptr;
    Ogre::SceneNode*           rootSceneNode = nullptr;
    Ogre::Camera*              camera        = nullptr;
    Ogre::Window*              window        = nullptr;
    Ogre::CompositorWorkspace* workspace     = nullptr;
  };

}
