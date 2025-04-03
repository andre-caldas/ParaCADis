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

#include "GlThreadQueue.h"

#include <exception>
#include <iostream>

namespace Mesh
{
  bool GlThreadQueue::frameStarted(const Ogre::FrameEvent& evt)
  {
    // I know... queue.empty() is not thread safe.
    // But we have no problems with spurious fail.
    while(!queue.empty()) {
      // Does not block.
      auto callback = queue.try_pull();
      if(!callback) {
        break;
      }
      try {
        (*callback)();
      } catch(const std::exception& e) {
        std::cerr << "Exception caught in rednering queue: " << e.what() << ".\n";
      } catch(...) {
        std::cerr << "Unkown exception caught in rendering queue.\n";
      }
    }
    return true;
  }
}
