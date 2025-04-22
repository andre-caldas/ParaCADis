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
   * Center and radius.
   */
  struct SphereCenterRadius
  {
    FloatPoint3D center;
    float        radius;
    bool operator==(const SphereCenterRadius&) const = default;
  };

  template<>
  class Description<SphereCenterRadius>
    : public DescriptionT<SphereCenterRadius, "Sphere",
    {&SphereCenterRadius::radius, "Radius"},
    {&SphereCenterRadius::center, "Center"}>
  {};



  /*
   * Center and point.
   */
  struct SphereCenterSurfacePoint
  {
    FloatPoint3D center;
    FloatPoint3D surface_point;
    bool operator==(const SphereCenterSurfacePoint&) const = default;
  };

  template<>
  class Description<SphereCenterSurfacePoint>
    : public DescriptionT<SphereCenterSurfacePoint, "Sphere (two points)",
    {&SphereCenterSurfacePoint::center, "Center"},
    {&SphereCenterSurfacePoint::surface_point, "Surface point"}>
  {};
}
