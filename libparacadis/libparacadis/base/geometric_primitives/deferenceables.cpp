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

#include <libparacadis/base/threads/locks/reader_locks.h>

DeferenceablePoint::DeferenceablePoint()
    : Exporter(Threads::MutexLayer{10})
{
}

DeferenceablePoint::DeferenceablePoint(const Point& p)
    : Exporter(Threads::MutexLayer{10}, p.x(), p.y(), p.z())
{
}

DeferenceablePoint::DeferenceablePoint(Real x, Real y, Real z)
    : Exporter(Threads::MutexLayer{10}, std::move(x), std::move(y), std::move(z))
{
}

DeferenceablePoint::operator Point() const noexcept
{
  Threads::ReaderGate gate{*this};
  return Point{gate->x, gate->y, gate->z};
}

SharedPtr<DeferenceablePoint> DeferenceablePoint::deepCopy() const
{
  return SharedPtrWrap<DeferenceablePoint>(operator Point());
}


DeferenceableVector::DeferenceableVector()
    : Exporter(Threads::MutexLayer{10})
{
}

DeferenceableVector::DeferenceableVector(const Vector& v)
    : Exporter(Threads::MutexLayer{10}, v.x(), v.y(), v.z())
{
}

DeferenceableVector::DeferenceableVector(Real x, Real y, Real z)
    : Exporter(Threads::MutexLayer{10}, std::move(x), std::move(y), std::move(z))
{
}

DeferenceableVector::operator Vector() const noexcept
{
  Threads::ReaderGate gate{*this};
  return Vector{gate->x, gate->y, gate->z};
}

SharedPtr<DeferenceableVector> DeferenceableVector::deepCopy() const
{
  return SharedPtrWrap<DeferenceableVector>(operator Vector());
}


/**
 * Template instantiation.
 * There is no need to include NameSearch_impl.h for those.
 */
#include <libparacadis/base/naming/IExport.h>
#include <libparacadis/base/naming/NameSearch.h>

template class Naming::IExport<DeferenceablePoint>;
template class Naming::IExport<DeferenceableVector>;

template class Naming::NameSearch<DeferenceablePoint>;
template class Naming::NameSearch<DeferenceableVector>;
