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

#include "gates.h"

#include "LockPolicy.h"

using namespace Threads;

template<typename LockType>
_GateBase<LockType>::_GateBase(MutexData& mutex)
    : mutex(mutex)
    , lock(std::make_unique<LockType>(mutex))
{
}

template<typename LockType>
void _GateBase<LockType>::release()
{
    assert(lock);
    lock.reset();
}

template<typename LockType>
void _GateBase<LockType>::resume()
{
    assert(!lock);
    lock = std::make_unique<LockType>(mutex);
}
