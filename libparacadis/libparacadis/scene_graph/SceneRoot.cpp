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

#include "CameraWrapper.h"
#include "ContainerNode.h"
#include "RenderingScope.h"

#include <libparacadis/base/threads/utils.h>

#include <cassert>
#include <memory>

#include <filament/Engine.h>
#include <filament/Renderer.h>
#include <filament/Scene.h>
#include <filament/View.h>

using namespace filament;

namespace SceneGraph
{
  SceneRoot::SceneRoot(void* native_window_handler)
      : signalQueue(std::make_shared<Threads::SignalQueue>())
      , renderingScope(std::make_shared<RenderingScope>())
  {
    engine     = Engine::create();
    swap_chain = engine->createSwapChain(native_window_handler);
    renderer   = engine->createRenderer();
    scene      = engine->createScene();
  }

  SceneRoot::~SceneRoot()
  {
    // Unfortunately, Filament does not like smart pointers... :-(
    cameras.clear();
    engine->destroy(scene);
    engine->destroy(renderer);
    engine->destroy(swap_chain);
    Engine::destroy(engine);
  }


  void SceneRoot::populate(const SharedPtr<SceneRoot>& self,
                           const SharedPtr<Document::DocumentTree>& document)
  {
    self->self = self;
    self->rootContainer = ContainerNode::create_root_node(self, document);
  }

  void SceneRoot::startRenderingThread()
  {
    auto lambda = [queue = signalQueue, self = self.lock()](){
      auto renderer = self->renderer;
      auto swap_chain = self->swap_chain;
      if(renderer->beginFrame(swap_chain)) {
        // TODO: investigate a good number instead of 16.
        // Ideally, we should know how long rendering took and do it after that.
        // But I've heard `endFrame()` blocks... :-(
        queue->try_run(std::chrono::milliseconds{16});
        renderer->endFrame();
      }
    };

    std::thread thread{std::move(lambda)};
    Threads::set_thread_name(thread, "Rendering Thread");
    thread.detach();
  }
}
