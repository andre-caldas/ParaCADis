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

#pragma once

#include "deferenceables.h"
#include "types.h"

#include <base/naming_scheme/Chainables.h>
#include <base/naming_scheme/IExport.h>

/**
 * DataStruct for ShpereCenterRadius2.
 */
struct SphereCenterRadius2Data {
  DeferenceablePoint center;
  Real               radius2;
};

/**
 * A sphere determined by its center and the squared radius.
 */
class SphereCenterRadius2
    : public NamingScheme::Exporter<SphereCenterRadius2Data>
    , public NamingScheme::IExportStruct<DeferenceablePoint, SphereCenterRadius2Data,
                                {&SphereCenterRadius2Data::center, "center"},
                                {&SphereCenterRadius2Data::center, "c"}>
    , public NamingScheme::IExportStruct<Real, SphereCenterRadius2Data,
                                {&SphereCenterRadius2Data::radius2, "radius2"},
                                {&SphereCenterRadius2Data::radius2, "r2"}>
    , public NamingScheme::Chainables<DeferenceablePoint>
{
public:
  SphereCenterRadius2(Point center, Real radius2);

  std::unique_ptr<SphereCenterRadius2> deepCopy() const;
  std::unique_ptr<NamingScheme::ExporterBase> deepCopyExporter() const override
  { return deepCopy(); }
};


/**
 * DataStruct for SphereCenterAndSurfacePoint.
 */
struct SphereCenterAndSurfacePointData {
  DeferenceablePoint center;
  DeferenceablePoint surface_point;
};

/**
 * A sphere determined by its center and a point on its surface.
 */
class SphereCenterAndSurfacePoint
    : public NamingScheme::Exporter<SphereCenterAndSurfacePointData>
    , public NamingScheme::IExportStruct<DeferenceablePoint, SphereCenterAndSurfacePointData,
                                {&SphereCenterAndSurfacePointData::center, "center"},
                                {&SphereCenterAndSurfacePointData::center, "c"}>
    , public NamingScheme::IExportStruct<DeferenceablePoint, SphereCenterAndSurfacePointData,
                                {&SphereCenterAndSurfacePointData::surface_point, "surface_point"},
                                {&SphereCenterAndSurfacePointData::surface_point, "p"}>
    , public NamingScheme::Chainables<DeferenceablePoint>
{
public:
  SphereCenterAndSurfacePoint(Point center, Point surface_point);

  std::unique_ptr<SphereCenterAndSurfacePoint> deepCopy() const;
  std::unique_ptr<NamingScheme::ExporterBase> deepCopyExporter() const override
  { return deepCopy(); }
};
