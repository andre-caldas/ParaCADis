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

#include "deferenceables_description.h"

#include <libparacadis/base/data_description/Description.h>

namespace DataDescription
{
  /*
   * Radius, center and normal vector.
   */
  struct CircleRadiusCenterNormal
  {
    float radius;
    FloatPoint3D center;
    FloatVector3D normal;
    bool operator==(const CircleRadiusCenterNormal&) const = default;
  };

  template<>
  class Description<CircleRadiusCenterNormal>
    : public DescriptionT<CircleRadiusCenterNormal, "Circle",
    {&CircleRadiusCenterNormal::radius, "Radius"},
    {&CircleRadiusCenterNormal::center, "Center"},
    {&CircleRadiusCenterNormal::normal, "Normal"}>
  {};


  /*
   * Three points.
   */
  struct Circle3Points
  {
    FloatPoint3D a, b, c;
    bool operator==(const Circle3Points&) const = default;
  };

  template<>
  class Description<Circle3Points>
    : public DescriptionT<Circle3Points, "Circle (three points)",
    {&Circle3Points::a, "A"},
    {&Circle3Points::b, "B"},
    {&Circle3Points::c, "C"}>
  {};
}
