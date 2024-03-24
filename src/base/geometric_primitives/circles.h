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

#include "deferenceables.h"
#include "types.h"

#include <base/naming_scheme/IExport.h>

/**
 * DataStruct for CircleRadius.
 */
struct CirclePointRadiusData {
  DeferenceablePoint center;
  Real               radius;
};

/**
 * A counter-clockwise oriented circle, if radius is positive.
 */
class CirclePointRadius
    : NamingScheme::SafeExporter<CirclePointRadiusData>
    , NamingScheme::SafeIExport<DeferenceablePoint, CirclePointRadiusData,
                                {&CirclePointRadiusData::center, "center"},
                                {&CirclePointRadiusData::center, "c"}>
    , NamingScheme::SafeIExport<Real, CirclePointRadiusData,
                                {&CirclePointRadiusData::radius, "radius"},
                                {&CirclePointRadiusData::radius, "r2"}>
{
public:
  CirclePointRadius(Point center, Real radius);
};


/**
 * DataStruct for Circle3Points.
 */
struct Circle3PointsData {
  DeferenceablePoint a;
  DeferenceablePoint b;
  DeferenceablePoint c;
};

/**
 * An oriented circle defined by three points.
 * Orientation given by the oriented triangle: a --> b --> c.
 */
class Circle3Points
    : NamingScheme::SafeExporter<Circle3PointsData>
    , NamingScheme::SafeIExport<DeferenceablePoint, Circle3PointsData,
                                {&Circle3PointsData::a, "a"},
                                {&Circle3PointsData::a, "p1"},
                                {&Circle3PointsData::b, "b"},
                                {&Circle3PointsData::b, "p2"},
                                {&Circle3PointsData::c, "c"},
                                {&Circle3PointsData::c, "p3"}>
{
public:
  Circle3Points(Point a, Point b, Point c);
};


/**
 * DataStruct for Circle2PointsDirection.
 */
struct Circle2PointsDirectionData {
  DeferenceablePoint  a;
  DeferenceablePoint  b;
  DeferenceableVector d;
};

/**
 * An oriented circle defined by two points and the "distance vector" to the center.
 *
 * Take the oriented line segment a --> b.
 * Rotate it 90 degrees counter-clockwise. Place it over the midpoint (a+b)/2.
 * Walk the provided distance and you are standing over the circle center.
 */
class Circle2PointsDirection
    : NamingScheme::SafeExporter<Circle2PointsDirectionData>
    , NamingScheme::SafeIExport<DeferenceablePoint, Circle2PointsDirectionData,
                                {&Circle2PointsDirectionData::a, "a"},
                                {&Circle2PointsDirectionData::a, "p1"},
                                {&Circle2PointsDirectionData::b, "b"},
                                {&Circle2PointsDirectionData::b, "p2"}>
    , NamingScheme::SafeIExport<DeferenceableVector, Circle2PointsDirectionData,
                                {&Circle2PointsDirectionData::d, "d"},
                                {&Circle2PointsDirectionData::d, "to_center"}>
{
public:
  Circle2PointsDirection(Point a, Point b, Vector chord_to_center);
};

#endif
