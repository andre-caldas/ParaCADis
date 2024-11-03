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

#include "CoordinateSystem.h"

#include "DeferenceableCoordinates.h"
#include "../deferenceables.h"
#include "../types.h"

#include <base/expected_behaviour/SharedPtrWrap.h>
#include <base/naming_scheme/Chainables.h>

/**
 * DataStruct for orthogonal coordinate system.
 */
struct CartesianCoordinatePointsData {
  SharedPtrWrap<DeferenceablePoint> origin = {0, 0, 0};
  SharedPtrWrap<DeferenceablePoint> p1     = {1, 0, 0};
  SharedPtrWrap<DeferenceablePoint> p2     = {0, 1, 0};
};

template<CoordinateSystem::AxisOrder order, TemplateString name_p1, TemplateString name_p2>
class DeferenceableCoordinatePoints
    : public NamingScheme::Exporter<CartesianCoordinatePointsData>
    , public DeferenceableCoordinates
    , public NamingScheme::IExportStruct<DeferenceablePoint, CartesianCoordinatePointsData,
                                {&CartesianCoordinatePointsData::origin, "origin"},
                                {&CartesianCoordinatePointsData::origin, "o"},
                                {&CartesianCoordinatePointsData::p1, "p1"},
                                {&CartesianCoordinatePointsData::p1, name_p1},
                                {&CartesianCoordinatePointsData::p2, "p2"},
                                {&CartesianCoordinatePointsData::p2, name_p2}>
    , public NamingScheme::Chainables<DeferenceablePoint>
{
  ONLY_SHAREDPTRWRAP()
  DeferenceableCoordinatePoints() = default;
  DeferenceableCoordinatePoints(Point origin);
  DeferenceableCoordinatePoints(Point origin, Point p1, Point p2);
public:
  CoordinateSystem getCoordinateSystem() const override;

  SharedPtr<DeferenceableCoordinatePoints> deepCopy() const;
  SharedPtr<NamingScheme::ExporterBase> deepCopyExporter() const override
  { return deepCopy(); }
};


using DeferenceableCoordinatePointsXY =
    DeferenceableCoordinatePoints<CoordinateSystem::AxisOrder::XY, {"a"}, {"b"}>;

using DeferenceableCoordinatePointsYX =
    DeferenceableCoordinatePoints<CoordinateSystem::AxisOrder::YX, {"b"}, {"a"}>;

using DeferenceableCoordinatePointsZX =
    DeferenceableCoordinatePoints<CoordinateSystem::AxisOrder::ZX, {"c"}, {"a"}>;

using DeferenceableCoordinatePointsZY =
    DeferenceableCoordinatePoints<CoordinateSystem::AxisOrder::ZY, {"c"}, {"b"}>;
