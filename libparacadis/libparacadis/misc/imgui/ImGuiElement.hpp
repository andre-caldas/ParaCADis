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

#include "ImGuiElement.h"

#include <libparacadis/base/geometric_primitives/deferenceables_description.h>

#include <imgui.h>
#include <type_traits>

namespace ParacadisImGui
{
  template<>
  template<>
  bool ImGuiElement<float>::draw(const char* label, T& value)
  {
    ImGui::InputFloat(label, &value);
    return true;
  }

  template<typename T>
  concept C_FloatArray3 =
      std::is_standard_layout_v<T> && sizeof(T) == sizeof(float) * 3;

  template<C_FloatArray3 T>
  template<>
  bool ImGuiElement<T>::draw(const char* label, T& value)
  {
    ImGui::InputFoat3(label, &value);
    return true;
  }
}
