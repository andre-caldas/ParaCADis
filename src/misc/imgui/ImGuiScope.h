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

#pragma once

#include <base/threads/locks/unreliable_mirror.h>
#include <base/threads/dedicated_thread_scope/DedicatedThreadScope.h>

#include <OGRE/Bites/OgreImGuiInputListener.h>

namespace OgreBites {
  class InputListenerChain;
}

namespace ParacadisImGui
{
  class ImGuiScope
    : public Threads::DedicatedThreadScopeBase
  {
  public:
    ImGuiScope();

    template<Threads::C_MutexHolderWithGates Holder>
    using Mirror = Threads::UnreliableMirrorGate<Holder>;

    template<Threads::C_MutexHolderWithGates Holder>
    void addMutexHolder(SharedPtr<Holder> holder, std::function<bool(Mirror<Holder>&)> f);

    template<Threads::C_MutexHolderWithGates Holder>
    void addMirror(SharedPtr<Mirror<Holder>> mirror, std::function<bool(Mirror<Holder>&)> f);

    /**
     * Just to make python bindings easier.
     * Probably we want to remove this in the future.
     */
    void addToInputListenerChain(OgreBites::InputListenerChain* chain);
    OgreBites::ImGuiInputListener input_listener;

  protected:
    void execute() noexcept override;
  };
}
