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

#include "deferenceables.h"
#include "deferenceables_description.h"

#include <libparacadis/base/data_description/Description.h>

namespace DataDescription
{
  struct FloatPoint3D
  {
    float x = 0;
    float y = 0;
    float z = 0;
    bool operator==(const FloatPoint3D&) const = default;
  };

  struct FloatVector3D
  {
    float x = 0;
    float y = 0;
    float z = 0;
    bool operator==(const FloatVector3D&) const = default;
  };

  template<>
  class Description<FloatPoint3D>
    : public DescriptionT<FloatPoint3D, "3D point",
    {&FloatPoint3D::x, "x"},
    {&FloatPoint3D::y, "y"},
    {&FloatPoint3D::z, "z"}>
  {};

  template<>
  class Description<FloatVector3D>
    : public DescriptionT<FloatVector3D, "3D vector",
    {&FloatVector3D::x, "x"},
    {&FloatVector3D::y, "y"},
    {&FloatVector3D::z, "z"}>
  {};
}
