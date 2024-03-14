// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2023-2024 André Caldas <andre.em.caldas@gmail.com>       *
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

#include "Exporter.h"

#include <base/threads/locks/LockPolicy.h>

#include <memory>

using namespace Threads;

namespace NamingScheme
{

  SharedLock ResolvedDataBase::lockForReading() const
  {
    return SharedLock(exporter->getMutexData());
  }

  ExclusiveLock<MutexData> ResolvedDataBase::lockForWriting() const
  {
    return ExclusiveLock(exporter->getMutexData());
  }

}  // namespace NamingScheme

/**
 * Template instantiation for most used exported types.
 */

#include <base/geometric_primitives/Circle.h>
#include <base/geometric_primitives/Line.h>
#include <base/geometric_primitives/types.h>

template class NamingScheme::ResolvedData<Real>;
template class NamingScheme::ResolvedData<Point>;
template class NamingScheme::ResolvedData<Vector>;
template class NamingScheme::ResolvedData<Line>;
template class NamingScheme::ResolvedData<Circle>;

