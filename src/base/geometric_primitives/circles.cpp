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

#include "circles.h"

CirclePointRadiusNormal::CirclePointRadiusNormal(Point center, Real radius, Vector normal)
    : Exporter{std::move(center), std::move(normal), std::move(radius)}
{
}


Circle3Points::Circle3Points(Point a, Point b, Point c)
    : Exporter{std::move(a), std::move(b), std::move(c)}
{
}


Circle2PointsDirection::Circle2PointsDirection(Point a, Point b, Vector d)
    : Exporter{std::move(a), std::move(b), std::move(d)}
{
}

/**
 * Template instantiation.
 * There is no need to include NameSearchResult_impl.h for those.
 */
#include <base/naming_scheme/IExport.h>
#include <base/naming_scheme/IExport_impl.h>
#include <base/naming_scheme/NameSearchResult.h>
#include <base/naming_scheme/NameSearchResult_impl.h>

template class NamingScheme::IExport<CirclePointRadiusNormal>;
template class NamingScheme::IExport<Circle3Points>;
template class NamingScheme::IExport<Circle2PointsDirection>;

template class NamingScheme::NameSearchResult<CirclePointRadiusNormal>;
template class NamingScheme::NameSearchResult<Circle3Points>;
template class NamingScheme::NameSearchResult<Circle2PointsDirection>;
