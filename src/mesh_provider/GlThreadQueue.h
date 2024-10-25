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

#include <base/threads/safe_structs/ThreadSafeQueue.h>
#include <OGRE/OgreFrameListener.h>

#include <functional>

namespace Mesh
{
  /**
   * GL operations can only be made in the thread that holds the GL context.
   * Therefore, we register this frame listener that holds a queue of
   * closures (lambdas) that will be executed in this thread.
   *
   * @attention
   * Ideally, only the GL part should be executed by those lambdas.
   */
  class GlThreadQueue
      : public Ogre::FrameListener
  {
  public:
    bool frameEnded(const Ogre::FrameEvent& evt) override;
    Threads::SafeStructs::ThreadSafeQueue<std::function<void()>> queue;
  };
}
