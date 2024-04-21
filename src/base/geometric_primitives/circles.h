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
struct CirclePointRadiusNormalData {
  DeferenceablePoint  center;
  DeferenceableVector normal;
  Real                radius;
};

/**
 * A counter-clockwise oriented circle, thumbs up (right-hand).
 */
class CirclePointRadiusNormal
    : public NamingScheme::Exporter<CirclePointRadiusNormalData>
    , public NamingScheme::IExportStruct<DeferenceablePoint, CirclePointRadiusNormalData,
                                {&CirclePointRadiusNormalData::center, "center"},
                                {&CirclePointRadiusNormalData::center, "c"}>
    , public NamingScheme::IExportStruct<DeferenceableVector, CirclePointRadiusNormalData,
                                {&CirclePointRadiusNormalData::normal, "normal"},
                                {&CirclePointRadiusNormalData::normal, "n"}>
    , public NamingScheme::IExportStruct<Real, CirclePointRadiusNormalData,
                                {&CirclePointRadiusNormalData::radius, "radius"},
                                {&CirclePointRadiusNormalData::radius, "r2"}>
    , public NamingScheme::Chainables<DeferenceablePoint, DeferenceableVector>
{
public:
  CirclePointRadiusNormal(Point center, Real radius, Vector normal = {1,0,0});

  std::unique_ptr<CirclePointRadiusNormal> deepCopy() const;
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
