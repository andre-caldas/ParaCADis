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

#include <libparacadis/base/data_description/GateTranslator.h>
#include <libparacadis/base/threads/dedicated_thread_scope/DedicatedThreadScope.h>

#include <functional>

namespace ParacadisImGui
{
  class ImGuiScope
    : public Threads::DedicatedThreadScopeBase
  {
  public:
    ImGuiScope();

    template<Threads::C_MutexHolderWithGates Holder>
    using Translator = DataDescription::GateTranslator<Holder>;

    template<Threads::C_MutexHolderWithGates Holder, typename Func>
    void addMutexHolder(SharedPtr<Holder> holder, Func&& f);

    template<Threads::C_MutexHolderWithGates Holder, typename Func>
    void addTranslator(SharedPtr<Translator<Holder>>, Func&& f);

  protected:
    void execute() noexcept override;
  };
}

#include "ImGuiScope.hpp"
