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

#ifndef GeometricPrimitives_Circles
#define GeometricPrimitives_Circles

#include "DeferenceablePoint.h"
#include "types.h"

#include <base/naming_scheme/IExport.h>

/**
 * A counter-clockwise oriented circle, if radius is positive.
 */
class CircleRadiusPoint
    : IExport<DeferenceablePoint>
    , IExport<Real>
{
public:
  Exported<DeferenceablePoint, "center"> center;
  Exported<Real, "radius"> radius;

  CircleRadiusPoint(const Point& center, Real radius);
};


/**
 * An oriented circle defined by three points.
 * Orientation given by the points: a --> b --> c.
 */
class Circle3Points : IExport<DeferenceablePoint>
{
public:
  Exported<DeferenceablePoint, "a"> a;
  Exported<DeferenceablePoint, "b"> b;
  Exported<DeferenceablePoint, "c"> c;

  Circle3Points(const Point& a, const Point& b, const Point& c);
};


/**
 * An oriented circle defined by two points and one distance.
 *
 * Take the oriented line segment a --> b.
 * Rotate it 90 degrees counter-clockwise. Place it over the midpoint (a+b)/2.
 * Walk the provided distance and you are standing over the circle center.
 */
class Circle2Points
    : IExport<DeferenceablePoint>
    , IExport<Real>
{
public:
  Exported<DeferenceablePoint, "a"> a;
  Exported<DeferenceablePoint, "b"> b;
  Exported<Real, "chord_to_center"> chord_to_center;

  Circle3Points(const Point& a, const Point& b, Real chord_to_center);
};

#endif

