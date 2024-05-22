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

CirclePointRadius2Normal::CirclePointRadius2Normal(Point center, Real radius2, Vector normal)
    : Exporter{{std::move(center), std::move(normal), std::move(radius2)}}
{
}

std::unique_ptr<CirclePointRadius2Normal> CirclePointRadius2Normal::deepCopy() const
{
  Threads::ReaderGate gate{*this};
  return std::make_unique<CirclePointRadius2Normal>(gate->center, gate->radius2, gate->normal);
}


Circle3Points::Circle3Points(Point a, Point b, Point c)
    : Exporter{{std::move(a), std::move(b), std::move(c)}}
{
}

std::unique_ptr<Circle3Points> Circle3Points::deepCopy() const
{
  Threads::ReaderGate gate{*this};
  return std::make_unique<Circle3Points>(gate->a, gate->b, gate->c);
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

template class IExport<CirclePointRadius2Normal>;
template class IExport<Circle3Points>;

template class NameSearch<CirclePointRadius2Normal>;
template class NameSearch<Circle3Points>;
