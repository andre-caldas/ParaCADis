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

#include <base/naming_scheme/Chainables.h>

/**
 * DataStruct for orthogonal coordinate system.
 */
struct CartesianCoordinateSystemData {
  DeferenceablePoint  origin = {0, 0, 0};
  DeferenceableVector v1     = {1, 0, 0};
  DeferenceableVector v2     = {0, 1, 0};
};

template<CoordinateSystem::AxisOrder order, TemplateString name_v1, TemplateString name_v2>
class DeferenceableCoordinateSystem
    : public NamingScheme::Exporter<CartesianCoordinateSystemData, DeferenceableCoordinates>
    , public NamingScheme::IExportStruct<DeferenceablePoint, CartesianCoordinateSystemData,
                                {&CartesianCoordinateSystemData::origin, "origin"},
                                {&CartesianCoordinateSystemData::origin, "o"}>
    , public NamingScheme::IExportStruct<DeferenceableVector, CartesianCoordinateSystemData,
                                {&CartesianCoordinateSystemData::v1, "v1"},
                                {&CartesianCoordinateSystemData::v1, name_v1},
                                {&CartesianCoordinateSystemData::v2, "v2"},
                                {&CartesianCoordinateSystemData::v2, name_v2}>
    , public NamingScheme::Chainables<DeferenceablePoint, DeferenceableVector>
{
public:
  DeferenceableCoordinateSystem() = default;
  DeferenceableCoordinateSystem(Point origin);
  DeferenceableCoordinateSystem(Point origin, Vector v1, Vector v2);
  CoordinateSystem getCoordinateSystem() const;

  std::unique_ptr<DeferenceableCoordinateSystem> deepCopy() const;
  std::unique_ptr<NamingScheme::ExporterBase> deepCopyExporter() const override
  { return deepCopy(); }
};


using DeferenceableCoordinateSystemXY =
    DeferenceableCoordinateSystem<CoordinateSystem::AxisOrder::XY, {"x"}, {"y"}>;

using DeferenceableCoordinateSystemYX =
    DeferenceableCoordinateSystem<CoordinateSystem::AxisOrder::YX, {"y"}, {"x"}>;

using DeferenceableCoordinateSystemZX =
    DeferenceableCoordinateSystem<CoordinateSystem::AxisOrder::ZX, {"z"}, {"x"}>;

using DeferenceableCoordinateSystemZY =
    DeferenceableCoordinateSystem<CoordinateSystem::AxisOrder::ZY, {"z"}, {"y"}>;
