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

#ifndef Threads_WriterLocks_impl_H
#define Threads_WriterLocks_impl_H

#include "writer_locks.h"

#include "MutexesWithPivot.h"

#include <map>

namespace Threads {

template<C_MutexGatherOrData... Mutex>
ExclusiveLock<Mutex...>::ExclusiveLock(Mutex&... mutexes)
    : LockPolicy(true, mutexes...)
{
  using GatherThemAll = GatherMutexData<Mutex...>;

  // Locks non-exclusivelly, according to LockPolicy.
  std::multimap<int, MutexData*> all_mutexes;
  for(auto m: getPivotMutexes()) {
    all_mutexes.emplace(m->layer, m);
  }
  pivot_shared_locks.reserve(all_mutexes.size());
  for(auto [l, m]: all_mutexes) {
    pivot_shared_locks.emplace_back(std::shared_lock(m->mutex));
  }

  /*
   * First you have to lock everything you are going to use.
   * ExclusiveLock l1(m1, m2);
   * ExclusiveLock l2(m1); // Does nothing. (inside some method)
   */
  assert(getMainMutexes().empty()
         || getMainMutexes().size() == GatherThemAll::nMutexes());
  if (!getMainMutexes().empty()) {
    locks = lockAllExclusive(mutexes...);
  }
}

template<C_MutexGatherOrData... Mutex>
void ExclusiveLock<Mutex...>::release()
{
  locks.reset();
}


template<C_MutexHolderWithGates Holder>
WriterGate<Holder>::WriterGate(Holder& holder)
    : ExclusiveLock<typename Holder::mutex_data_t>(getMutex(holder))
    , data(Holder::WriterGate::getProtectedData(holder))
{
}


template<C_MutexHolderWithGates ... Holders>
WriterGate<Holders...>::WriterGate(Holders&... holders)
    : ExclusiveLock<typename Holders::mutex_data_t...>(getMutex(holders)...)
#ifndef NDEBUG
    , all_holders{&holders...}
#endif
{
}

template<C_MutexHolderWithGates ... Holders>
template<C_MutexHolderWithGates Holder>
auto& WriterGate<Holders...>::operator[](Holder& holder) const
{
  assert(all_holders.contains(&holder));
  return Holder::WriterGate::getProtectedData(holder);
}


template<C_MutexHolder Holder, typename T, T Holder::* localData>
LocalWriterGate<localData>::LocalWriterGate(Holder& holder)
    : _GateBase(holder)
    , data(holder.*localData)
{}

}

#endif
