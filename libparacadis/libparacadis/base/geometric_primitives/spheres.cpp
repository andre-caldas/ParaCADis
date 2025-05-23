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

#include "spheres.h"

#include <gismo/gismo.h>

using namespace Document;

/*
 * SphereCenterRadius2
 */
SphereCenterRadius2::SphereCenterRadius2(Point center, Real radius2)
    : Exporter(std::move(center), std::move(radius2))
{
}

SharedPtr<SphereCenterRadius2> SphereCenterRadius2::deepCopy() const
{
  Threads::ReaderGate gate{*this};
  return SharedPtrWrap<SphereCenterRadius2>(gate->center, gate->radius2);
}

SharedPtr<const DocumentGeometry::iga_surface_t>
SphereCenterRadius2::produceIgaSurface() const
{
  Point  center;
  real_t radius2;
  {
    Threads::ReaderGate gate{*this};
    radius2 = types::to_float(gate->radius2);
    center = gate->center;
  }
  real_t x = types::to_float(center.x());
  real_t y = types::to_float(center.y());
  real_t z = types::to_float(center.z());
  return SharedPtr{gismo::gsNurbsCreator<real_t>::NurbsSphere(std::sqrt(radius2), x, y, z)};
}


SphereCenterSurfacePoint::SphereCenterSurfacePoint
    (Point center, Point surface_point)
    : Exporter(std::move(center), std::move(surface_point))
{
}

SharedPtr<SphereCenterSurfacePoint> SphereCenterSurfacePoint::deepCopy() const
{
  Threads::ReaderGate gate{*this};
  return SharedPtrWrap<SphereCenterSurfacePoint>(gate->center, gate->surface_point);
}

SharedPtr<const DocumentGeometry::iga_surface_t>
SphereCenterSurfacePoint::produceIgaSurface() const
{
  Point center;
  Point surface_point;
  {
    Threads::ReaderGate gate{*this};
    surface_point = gate->surface_point;
    center = gate->center;
  }
  real_t radius2 = types::to_float((surface_point-center).squared_length());
  real_t x = types::to_float(center.x());
  real_t y = types::to_float(center.y());
  real_t z = types::to_float(center.z());
  return SharedPtr{gismo::gsNurbsCreator<real_t>::NurbsSphere(std::sqrt(radius2), x, y, z)};
}


/**
 * Template instantiation.
 * There is no need to include NameSearch_impl.h for those.
 */
#include <libparacadis/base/naming/IExport.h>
#include <libparacadis/base/naming/NameSearch.h>

template class Naming::IExport<SphereCenterRadius2>;
template class Naming::IExport<SphereCenterSurfacePoint>;

template class Naming::NameSearch<SphereCenterRadius2>;
template class Naming::NameSearch<SphereCenterSurfacePoint>;
