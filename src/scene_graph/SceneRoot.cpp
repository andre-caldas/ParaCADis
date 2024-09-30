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

//#include "config.h"

#include "SceneRoot.h"

#include "ContainerNode.h"

#include <cassert>

#include <OGRE/OgreSceneManager.h>

#include <iostream>
namespace SceneGraph
{
  SceneRoot::SceneRoot(Ogre::SceneManager& scene_manager)
      : signalQueue(std::make_shared<Threads::SignalQueue>())
      , sceneManager(&scene_manager)
  {
  }


  void SceneRoot::populate(const SharedPtr<SceneRoot>& self,
                           const SharedPtr<Document::DocumentTree>& document)
  {
    self->self = self;
    self->rootContainer = ContainerNode::create_root_node(self, document);
  }

  void SceneRoot::runQueue()
  {
    signalQueue->run_thread(signalQueue);
  }
}
