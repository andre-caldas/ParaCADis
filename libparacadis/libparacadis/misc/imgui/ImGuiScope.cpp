// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2025 André Caldas <andre.em.caldas@gmail.com>            *
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

#include "ImGuiScope.h"

namespace ParacadisImGui
{
  ImGuiScope::ImGuiScope()
  {
    static bool first_run = true;
    assert(first_run && "Are you sure you need two ImGuiScope? I thought you wouldn't.");
    first_run = false;

#if 0
    auto* overlay_to_cast = Ogre::OverlayManager::getSingleton().getByName("ImGuiOverlay");
    assert(overlay_to_cast && "You need to initialize ImGuiOverlay, first.");
    if(overlay_to_cast) {
      auto* imguiOverlay = static_cast<Ogre::ImGuiOverlay*>(overlay_to_cast);
      imguiOverlay->setZOrder(300);
      imguiOverlay->show();
    }
#endif
  }

  void ImGuiScope::execute() noexcept
  {
    assert(false && "Implement!");
    DedicatedThreadScopeBase::execute();
  }
}
