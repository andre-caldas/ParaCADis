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

SphereCenterRadius2::SphereCenterRadius2(Point center, Real radius2)
    : Exporter{{std::move(center), std::move(radius2)}}
{
}

std::unique_ptr<SphereCenterRadius2> SphereCenterRadius2::deepCopy() const
{
  Threads::ReaderGate gate{*this};
  return std::make_unique<SphereCenterRadius2>(gate->center, gate->radius2);
}


SphereCenterAndSurfacePoint::SphereCenterAndSurfacePoint
    (Point center, Point surface_point)
    : Exporter{{std::move(center), std::move(surface_point)}}
{
}

std::unique_ptr<SphereCenterAndSurfacePoint> SphereCenterAndSurfacePoint::deepCopy() const
{
  Threads::ReaderGate gate{*this};
  return std::make_unique<SphereCenterAndSurfacePoint>(gate->center, gate->surface_point);
}


/**
 * Template instantiation.
 * There is no need to include NameSearch_impl.h for those.
 */
#include <base/naming_scheme/IExport.h>
#include <base/naming_scheme/IExport_impl.h>
#include <base/naming_scheme/NameSearch.h>
#include <base/naming_scheme/NameSearch_impl.h>

using namespace NamingScheme;

template class IExport<SphereCenterRadius2>;
template class IExport<SphereCenterAndSurfacePoint>;

template class NameSearch<SphereCenterRadius2>;
template class NameSearch<SphereCenterAndSurfacePoint>;
