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

#include "ImGuiScope.h"

namespace ParacadisImGui
{
  template<Threads::C_MutexHolderWithGates Holder, typename Func>
  void ImGuiScope::addMutexHolder(SharedPtr<Holder> holder, Func&& f)
  {
    static_assert(std::invocable<Func, Translator<Holder>&>,
                  "Function must be callable with Translator<Holder>& argument.");
    SharedPtrWrap<Translator<Holder>> translator(std::move(holder));
    addTranslator(SharedPtr{std::move(translator)}, std::move(f));
  }

  template<Threads::C_MutexHolderWithGates Holder, typename Func>
  void ImGuiScope::addTranslator(SharedPtr<Translator<Holder>> translator, Func&& f)
  {
    static_assert(std::invocable<Func, Translator<Holder>&>,
                  "Function must be callable with Translator<Holder>& argument.");
    auto lambda = [translator=std::move(translator), f=std::move(f)](){
      return f(*translator);
    };
    appendCallable(std::move(lambda));
  }
}
