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

#include <base/naming_scheme/Chainables.h>
#include <base/naming_scheme/IExport.h>

/**
 * DataStruct for CircleRadius.
 */
struct CirclePointRadius2NormalData {
  DeferenceablePoint  center;
  DeferenceableVector normal;
  Real                radius2;
};

/**
 * A counter-clockwise oriented circle, thumbs up (right-hand).
 */
class CirclePointRadius2Normal
    : public NamingScheme::Exporter<CirclePointRadius2NormalData>
    , public NamingScheme::IExportStruct<DeferenceablePoint, CirclePointRadius2NormalData,
                                {&CirclePointRadius2NormalData::center, "center"},
                                {&CirclePointRadius2NormalData::center, "c"}>
    , public NamingScheme::IExportStruct<DeferenceableVector, CirclePointRadius2NormalData,
                                {&CirclePointRadius2NormalData::normal, "normal"},
                                {&CirclePointRadius2NormalData::normal, "n"}>
    , public NamingScheme::IExportStruct<Real, CirclePointRadius2NormalData,
                                {&CirclePointRadius2NormalData::radius2, "radius2"},
                                {&CirclePointRadius2NormalData::radius2, "r2"}>
    , public NamingScheme::Chainables<DeferenceablePoint, DeferenceableVector>
{
public:
  CirclePointRadius2Normal(Point center, Real radius2, Vector normal = {1,0,0});

  std::unique_ptr<CirclePointRadius2Normal> deepCopy() const;
  std::unique_ptr<NamingScheme::ExporterBase> deepCopyExporter() const override
  { return deepCopy(); }
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
    : public NamingScheme::Exporter<Circle3PointsData>
    , public NamingScheme::IExportStruct<DeferenceablePoint, Circle3PointsData,
                                {&Circle3PointsData::a, "a"},
                                {&Circle3PointsData::a, "p1"},
                                {&Circle3PointsData::b, "b"},
                                {&Circle3PointsData::b, "p2"},
                                {&Circle3PointsData::c, "c"},
                                {&Circle3PointsData::c, "p3"}>
    , public NamingScheme::Chainables<DeferenceablePoint>
{
public:
  Circle3Points(Point a, Point b, Point c);

  std::unique_ptr<Circle3Points> deepCopy() const;
  std::unique_ptr<NamingScheme::ExporterBase> deepCopyExporter() const override
  { return deepCopy(); }
};

#endif
