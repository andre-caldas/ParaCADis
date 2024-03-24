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

#include "deferenceables.h"

DeferenceablePoint::DeferenceablePoint(const Point& p)
    : SafeExporter{p.x(), p.y(), p.z()}
{
}

DeferenceablePoint::DeferenceablePoint(Real x, Real y, Real z)
    : SafeExporter{std::move(x), std::move(y), std::move(z)}
{
}


DeferenceableVector::DeferenceableVector(const Vector& v)
    : SafeExporter{v.x(), v.y(), v.z()}
{
}

DeferenceableVector::DeferenceableVector(Real x, Real y, Real z)
    : SafeExporter{std::move(x), std::move(y), std::move(z)}
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

template class NamingScheme::IExport<DeferenceablePoint>;
template class NamingScheme::IExport<DeferenceableVector>;

template class NamingScheme::NameSearchResult<DeferenceablePoint>;
template class NamingScheme::NameSearchResult<DeferenceableVector>;
