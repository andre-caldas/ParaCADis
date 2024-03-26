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

#include "deferenceables.h"
#include "types.h"

#include <base/naming_scheme/Exporter.h>
#include <base/naming_scheme/IExport.h>
#include <base/threads/safe_structs/ThreadSafeStruct.h>


/**
 * DataStruct for Line2Points.
 */
struct Line2PointsData {
  DeferenceablePoint start;
  DeferenceablePoint end;
  bool is_bounded_start;
  bool is_bounded_end;
};

/**
 * An oriented line determined by two points.
 *
 * It can be bounded by the any of the two points.
 */
class Line2Points
    : public NamingScheme::Exporter<Line2PointsData>
    , public NamingScheme::SafeIExport<DeferenceablePoint, Line2PointsData,
                                {&Line2PointsData::start, "start"},
                                {&Line2PointsData::start, "a"},
                                {&Line2PointsData::end, "end"},
                                {&Line2PointsData::end, "b"}>
    , public NamingScheme::SafeIExport<bool, Line2PointsData,
                                {&Line2PointsData::is_bounded_start, "is_bounded_start"},
                                {&Line2PointsData::is_bounded_end, "is_bounded_end"}>
{
public:
  Line2Points(Point start, Point end);
//  std::string toString() const override;
};


/**
 * DataStruct for Line2Points.
 */
struct LinePointDirectionData {
  DeferenceablePoint start;
  DeferenceableVector direction;
  bool is_bounded_start;
  bool is_bounded_end;
};

/**
 * An oriented line determined by a (starting) point and a direction.
 *
 * It can be bounded in the starting point, or not.
 */
class LinePointDirection
    : public NamingScheme::Exporter<LinePointDirectionData>
    , public NamingScheme::SafeIExport<DeferenceablePoint, LinePointDirectionData,
                                {&LinePointDirectionData::start, "start"},
                                {&LinePointDirectionData::start, "a"}>
    , public NamingScheme::SafeIExport<DeferenceableVector, LinePointDirectionData,
                                {&LinePointDirectionData::direction, "direction"},
                                {&LinePointDirectionData::direction, "v"}>
    , public NamingScheme::SafeIExport<bool, LinePointDirectionData,
                                {&LinePointDirectionData::is_bounded_start, "is_bounded_start"},
                                {&LinePointDirectionData::is_bounded_end, "is_bounded_end"}>
{
public:
  LinePointDirection(Point start, Vector direction);
//  std::string toString() const override;
};

#endif
