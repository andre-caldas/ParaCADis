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

#include <base/type_traits/NamedMember.h>

namespace ParacadisImGui
{
  template<typename T>
  class ImGuiElement
  {
  public:
    /**
     * Actually creates ImGui elements.
     *
     * This is supposed to be specialized for each data type.
     *
     * @returns
     * Shall only return `false` when the dialog is supposed to be closed.
     * Otherwise, shall return `true`;
     */
    template<typename U = T>
    static bool draw(const char* label, T& value);
  };
}

#include "ImGuiElement.hpp"
