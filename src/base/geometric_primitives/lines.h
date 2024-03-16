// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2024 André Caldas <andre.em.caldas@gmail.com>            *
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

#ifndef GeometricPrimitives_Lines
#define GeometricPrimitives_Lines

#include "DeferenceablePoint.h"
#include "types.h"

#include <base/naming_scheme/IExport.h>

/**
 * An oriented line determined by two points.
 *
 * It can be bounded by the any of the two points.
 */
class Line2Points
    : IExport<DeferenceablePoint>
    , IExport<bool>
{
public:
  Exported<DeferenceablePoint, "start", "a"> start;
  Exported<DeferenceablePoint, "end", "b"> start;
  Exported<bool, "is_bounded_start"> is_bounded_start = false;
  Exported<bool, "is_bounded_end"> end = false;

  Line2Points(const Point& start, const Point& end, std::string name);
};

/**
 * An oriented line determined by a (starting) point and a direction.
 *
 * It can be bounded in the starting point, or not.
 */
class LinePointDirection
    : IExport<DeferenceablePoint>
    , IExport<Direction>
{
public:
  Exported<DeferenceablePoint, "start", "a"> start;
  Exported<DeferenceableDirection, "direction", "v"> direction;
  Exported<bool, "is_bounded_start"> is_bounded_start = false;
  Exported<bool, "is_bounded_end"> end = false;

  LinePointDirection(const Point& start, Direction direction, std::string name);
};

#endif

