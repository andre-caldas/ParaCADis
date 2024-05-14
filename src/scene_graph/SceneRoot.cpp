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

#include "SceneRoot.h"

#include "ContainerNode.h"

#include <cassert>

#include <OGRE/Compositor/OgreCompositorManager2.h>
#include <OGRE/Compositor/OgreCompositorWorkspace.h>
#include <OGRE/OgreCamera.h>
#include <OGRE/OgreWindow.h>

namespace SceneGraph
{

  SceneRoot::SceneRoot()
      : signalQueue(std::make_shared<Threads::SignalQueue>())
      , ogreRoot(std::make_unique<Ogre::Root>())
  {
    const char window_title[] = "ParaCADis window";
    ogreRoot->initialise(false, window_title);
    window = ogreRoot->createRenderWindow(window_title, 1024, 786, false);

    sceneManager = ogreRoot->createSceneManager(
        Ogre::ST_GENERIC, 2, "ParaCADis SceneManager");
    rootSceneNode = sceneManager->getRootSceneNode();

    camera = sceneManager->createCamera("The Camera!");
    camera->setAutoAspectRatio(true);
    camera->setNearClipDistance(0.2f);
    camera->setFarClipDistance(1000.0f);
    camera->setPosition({0, 20, 15});
    camera->lookAt({0, 0, 0});

    const char workspace_name[] = "ParaCADis workspace";
    auto* compositor_manager = ogreRoot->getCompositorManager2();
    if(!compositor_manager->hasWorkspaceDefinition(workspace_name)) {
      compositor_manager->createBasicWorkspaceDef(
          workspace_name, Ogre::ColourValue( 0.6f, 0.0f, 0.6f ));
    }
    workspace = compositor_manager->addWorkspace(
        sceneManager, window->getTexture(), camera, workspace_name, true);
  }


  void SceneRoot::populate(const SharedPtr<SceneRoot>& _self,
                           const SharedPtr<DocumentTree::DocumentTree>& document)
  {
    assert(this == _self.get());
    self = _self;
    rootContainer = ContainerNode::create_document(_self, document);
  }

  void SceneRoot::runQueue()
  {
    signalQueue->run_thread(signalQueue);
  }
}
