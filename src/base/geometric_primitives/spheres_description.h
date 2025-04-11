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
#include "DocumentGeometry.h"
#include "types.h"

#include <memory>
#include <atomic>

#include <base/expected_behaviour/SharedPtrWrap.h>
#include <base/naming_scheme/Chainables.h>
#include <base/naming_scheme/IExport.h>

#include <gismo/gismo.h>

/**
 * DataStruct for ShpereCenterRadius2.
 */
struct SphereCenterRadius2Data {
  SharedPtrWrap<DeferenceablePoint> center;
  Real                              radius2;
};

/**
 * A sphere determined by its center and the squared radius.
 */
class SphereCenterRadius2
    : public Document::DocumentSurface
    , public NamingScheme::Exporter<SphereCenterRadius2Data>
    , public NamingScheme::IExportStruct<DeferenceablePoint, SphereCenterRadius2Data,
                                {&SphereCenterRadius2Data::center, "center"},
                                {&SphereCenterRadius2Data::center, "c"}>
    , public NamingScheme::IExportStruct<Real, SphereCenterRadius2Data,
                                {&SphereCenterRadius2Data::radius2, "radius2"},
                                {&SphereCenterRadius2Data::radius2, "r2"}>
    , public NamingScheme::Chainables<DeferenceablePoint>
{
  ONLY_SHAREDPTRWRAP()
  SphereCenterRadius2(Point center, Real radius2);
public:
  SharedPtr<SphereCenterRadius2> deepCopy() const;
  SharedPtr<NamingScheme::ExporterBase> deepCopyExporter() const override
  { return deepCopy(); }

private:
  SharedPtr<const iga_surface_t> produceIgaSurface() const;
};


/**
 * DataStruct for SphereCenterSurfacePoint.
 */
struct SphereCenterSurfacePointData {
  SharedPtrWrap<DeferenceablePoint> center;
  SharedPtrWrap<DeferenceablePoint> surface_point;
};

/**
 * A sphere determined by its center and a point on its surface.
 */
class SphereCenterSurfacePoint
    : public Document::DocumentSurface
    , public NamingScheme::Exporter<SphereCenterSurfacePointData>
    , public NamingScheme::IExportStruct<DeferenceablePoint, SphereCenterSurfacePointData,
                                {&SphereCenterSurfacePointData::center, "center"},
                                {&SphereCenterSurfacePointData::center, "c"}>
    , public NamingScheme::IExportStruct<DeferenceablePoint, SphereCenterSurfacePointData,
                                {&SphereCenterSurfacePointData::surface_point, "surface_point"},
                                {&SphereCenterSurfacePointData::surface_point, "p"}>
    , public NamingScheme::Chainables<DeferenceablePoint>
{
  ONLY_SHAREDPTRWRAP()
  SphereCenterSurfacePoint(Point center, Point surface_point);
public:
  SharedPtr<SphereCenterSurfacePoint> deepCopy() const;
  SharedPtr<NamingScheme::ExporterBase> deepCopyExporter() const override
  { return deepCopy(); }

private:
  SharedPtr<const iga_surface_t> produceIgaSurface() const;
};
