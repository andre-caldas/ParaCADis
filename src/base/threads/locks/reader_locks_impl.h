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

#ifndef Threads_ReaderLocks_impl_H
#define Threads_ReaderLocks_impl_H

#include "reader_locks.h"

namespace Threads {

template<C_MutexGatherOrData Mutex>
SharedLock::SharedLock(Mutex& mutex)
    : LockPolicy(false, mutex)
{
  locks.reserve(getMutexes().size());
  for(auto m: getMutexes()) {
    locks.emplace_back(std::shared_lock(m->mutex));
  }
}


template<C_MutexHolderWithGates Holder>
ReaderGate<Holder>::ReaderGate(const Holder& holder)
    : SharedLock(getMutex(holder))
    , data(Holder::ReaderGate::getProtectedData(holder))
{
}


template<C_MutexHolderWithGates ... Holders>
ReaderGate<Holders...>::ReaderGate(const Holders&... holders)
    : locks{getMutex(holders)...}
#ifndef NDEBUG
    , all_holders{&holders...}
#endif
{
}

template<C_MutexHolderWithGates ... Holders>
template<C_MutexHolderWithGates Holder>
auto& ReaderGate<Holders...>::operator[](const Holder& holder) const
{
  assert(all_holders.contains(&holder));
  return Holder::ReaderGate::getProtectedData(holder);
}


template<C_MutexHolder Holder, typename T, T Holder::* localData>
LocalReaderGate<localData>::LocalReaderGate(const Holder& holder)
    : _GateBase(holder)
    , data(holder.*localData)
{}

template<C_MutexHolder Holder, typename T, T Holder::* localData>
const T& LocalReaderGate<localData>::operator*() const
{
  return data;
}

template<C_MutexHolder Holder, typename T, T Holder::* localData>
const T* LocalReaderGate<localData>::operator->() const
{
  return &data;
}

}
#endif
