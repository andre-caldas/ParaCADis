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

#include <base/data_description/Description.h>

namespace DataDescription
{
  struct Line2Points
  {
    FloatPoint3D start;
    FloatPoint3D end;
    bool is_bounded_start;
    bool is_bounded_end;
  };

  template<>
  class Description<Line2Points>
    : public DescriptionT<Line2Points, "Line",
    {&Line2Points::start, "Start"},
    {&Line2Points::end, "End"},
    {&Line2Points::is_bounded_start, "Bounded start"},
    {&Line2Points::is_bounded_end, "Bounded end"}>
  {};


  struct LinePointDirection
  {
    FloatPoint3D start;
    FloatVector3D direction;
    bool is_bounded_start;
    bool is_bounded_end;
  };

  template<>
  class Description<LinePointDirection>
    : public DescriptionT<LinePointDirection, "Line",
    {&LinePointDirection::start, "Start"},
    {&LinePointDirection::direction, "Direction"},
    {&LinePointDirection::is_bounded_start, "Bounded start"},
    {&LinePointDirection::is_bounded_end, "Bounded end"}>
  {};
}
