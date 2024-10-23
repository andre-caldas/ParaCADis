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

#include "DeferenceableCoordinatePoints.h"

#include "../checks.h"
#include "../exceptions.h"
#include "../types.h"

template<CoordinateSystem::AxisOrder order, TemplateString name_p1, TemplateString name_p2>
DeferenceableCoordinatePoints<order, name_p1, name_p2>::DeferenceableCoordinatePoints(Point origin)
    : Exporter{{std::move(origin)}}
{
}

template<CoordinateSystem::AxisOrder order, TemplateString name_p1, TemplateString name_p2>
DeferenceableCoordinatePoints<order, name_p1, name_p2>::DeferenceableCoordinatePoints(
  Point origin, Point p1, Point p2)
    : Exporter{{std::move(origin), std::move(p1), std::move(p2)}}
{
}

template<CoordinateSystem::AxisOrder order, TemplateString name_p1, TemplateString name_p2>
CoordinateSystem
DeferenceableCoordinatePoints<order, name_p1, name_p2>::getCoordinateSystem() const
{
  Point origin;
  Point p1;
  Point p2;
  { // Scoped lock
    Threads::ReaderGate gate{*this};
    origin = gate->origin;
    p1 = gate->p1;
    p2 = gate->p2;
  }
  return CoordinateSystem(std::move(origin), order, p1 - origin, p2 - origin);
}

template<CoordinateSystem::AxisOrder order, TemplateString name_p1, TemplateString name_p2>
std::unique_ptr<DeferenceableCoordinatePoints<order, name_p1, name_p2>>
DeferenceableCoordinatePoints<order, name_p1, name_p2>::deepCopy() const
{
  using coordinate_t = DeferenceableCoordinatePoints<order, name_p1, name_p2>;
  Point origin;
  Point p1;
  Point p2;
  { // Scoped lock
    Threads::ReaderGate gate{*this};
    origin = gate->origin;
    p1 = gate->p1;
    p2 = gate->p2;
  }
  return std::make_unique<coordinate_t>(std::move(origin), p1, p2);
}


template class DeferenceableCoordinatePoints<CoordinateSystem::AxisOrder::XY, {"a"}, {"b"}>;
template class DeferenceableCoordinatePoints<CoordinateSystem::AxisOrder::YX, {"b"}, {"a"}>;
template class DeferenceableCoordinatePoints<CoordinateSystem::AxisOrder::ZX, {"c"}, {"a"}>;
template class DeferenceableCoordinatePoints<CoordinateSystem::AxisOrder::ZY, {"c"}, {"b"}>;
