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

#include "DeferenceableCoordinateSystem.h"

#include "../checks.h"
#include "../exceptions.h"
#include "../types.h"

template<CoordinateSystem::AxisOrder order, TemplateString name_v1, TemplateString name_v2>
DeferenceableCoordinateSystem<order, name_v1, name_v2>::DeferenceableCoordinateSystem(Point origin)
    : Exporter(std::move(origin))
{
}

template<CoordinateSystem::AxisOrder order, TemplateString name_v1, TemplateString name_v2>
DeferenceableCoordinateSystem<order, name_v1, name_v2>::DeferenceableCoordinateSystem(
  Point origin, Vector v1, Vector v2)
    : Exporter(std::move(origin), std::move(v1), std::move(v2))
{
}

template<CoordinateSystem::AxisOrder order, TemplateString name_v1, TemplateString name_v2>
CoordinateSystem
DeferenceableCoordinateSystem<order, name_v1, name_v2>::getCoordinateSystem() const
{
  Threads::ReaderGate gate{*this};
  return CoordinateSystem(gate->origin, order, gate->v1, gate->v2);
}

template<CoordinateSystem::AxisOrder order, TemplateString name_v1, TemplateString name_v2>
std::unique_ptr<DeferenceableCoordinateSystem<order, name_v1, name_v2>>
DeferenceableCoordinateSystem<order, name_v1, name_v2>::deepCopy() const
{
  using coordinate_t = DeferenceableCoordinateSystem<order, name_v1, name_v2>;
  Point origin;
  Vector v1;
  Vector v2;
  { // Scoped lock
    Threads::ReaderGate gate{*this};
    origin = gate->origin;
    v1 = gate->v1;
    v2 = gate->v2;
  }
  return std::make_unique<coordinate_t>(std::move(origin), std::move(v1), std::move(v2));
}


template class DeferenceableCoordinateSystem<CoordinateSystem::AxisOrder::XY, {"x"}, {"y"}>;
template class DeferenceableCoordinateSystem<CoordinateSystem::AxisOrder::YX, {"y"}, {"x"}>;
template class DeferenceableCoordinateSystem<CoordinateSystem::AxisOrder::ZX, {"z"}, {"x"}>;
template class DeferenceableCoordinateSystem<CoordinateSystem::AxisOrder::ZY, {"z"}, {"y"}>;
