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

#include <memory>

using namespace Threads;

template<C_MutexData MData>
MutexData& getMutex(MData& mutex) { return mutex; }
template<C_MutexHolder Holder>
Holder::mutex_data_t& getMutex(const Holder& holder) { return holder; }

template<C_MutexGatherOrData FirstMutex, C_MutexGatherOrData... Mutex>
template<C_MutexHolder FirstHolder, C_MutexHolder... Holder>
ExclusiveLock<FirstMutex, Mutex...>::ExclusiveLock(FirstHolder& first_holder, Holder&... holder)
    : LockPolicy(true, getMutex(first_holder), getMutex(holder)...)
{
  using GatherThemAll = GatherMutexData<FirstMutex, Holder...>;

  /*
   * Here we know that if this is not the first lock,
   * all previous locks in the same layer were exclusive
   * and the very first one already contains this one.
   *    * If this is not the first one, mutexes is empty.
   * Otherwise, mutexes = {container.getMutexPtr()...}.
   *    * Example:
   * ExclusiveLock l1(m1, m2);
   * ExclusiveLock l2(m1); // Does nothing.
   */
  assert(getMutexes().empty() || getMutexes().size() == GatherThemAll::size());
  if (!getMutexes().empty()) {
    locks = lockThemAll(getMutex(first_holder), getMutex(holder)...);
  }
}

template<C_MutexGatherOrData FirstMutex, C_MutexGatherOrData... Mutex>
void ExclusiveLock<FirstMutex, Mutex...>::release()
{
  locks.reset();
}

template<C_MutexGatherOrData FirstMutex, C_MutexGatherOrData... Mutex>
auto ExclusiveLock<FirstMutex, Mutex...>::detachFromThread()
{
  LockPolicy::detachFromThread();
  return locks;
}


template<C_MutexHolder Holder, typename T, T Holder::* localData>
WriterGate<localData>::WriterGate(Holder& holder)
    : _GateBase(holder)
    , data(&(holder.*localData))
{}

#endif
