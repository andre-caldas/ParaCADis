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

#ifndef Threads_LockPolicy_inc_H
#define Threads_LockPolicy_inc_H

#include "LockPolicy.h"

#include "exceptions.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <mutex>
#include <shared_mutex>

namespace Threads
{

  template<C_MutexData... MutN>
  LockPolicy::LockPolicy(bool is_exclusive, MutN*... mutex)
      : mutexes{mutex...}
  {
    // Shared locks are acquired one by one.
    assert(!is_exclusive || mutexes.size() <= 1);
    _processLock(is_exclusive);
  }

  /*
   * Traits: we want to pass either a mutex or a container to ExclusiveLock.
   */
  template<typename C>
  struct MutexDataPointer {
    MutexDataPointer(const C& container)
        : container(container)
    {
    }

    auto getPair() { return container.getMutexData(); }

    const C& container;
  };

  template<C_MutexData FirstMutex, C_MutexData... MutexData>
  ExclusiveLock<FirstMutex, MutexData...>::ExclusiveLock(
      FirstMutex* first_mutex, MutexData*... mutex_data)
      : LockPolicy(true, first_mutex, mutex_data...)
  {
    /*
     * Here we know that if this is not the first lock,
     * all previous locks in the same layer were exclusive
     * and the very first one already contains this one.
     *
     * If this is not the first one, mutexes is empty.
     * Otherwise, mutexes = {container.getMutexPtr()...}.
     *
     * Example:
     * ExclusiveLock l1(m1, m2);
     * ExclusiveLock l2(m1); // Does nothing.
     */
    assert(
        getMutexes().empty() || getMutexes().size() == 1 + sizeof...(MutexData));
    if (getMutexes().size() == 1 + sizeof...(MutexData)) {
      /*
       * It would be more natural if we could pass "mutexes" to the constructor.
       * But there is only the option to list all mutexes at compile time.
       * Fortunately, mutexes = {container.getMutexPtr()...}.
       */
      locks = std::make_shared<locks_t>(first_mutex->mutex, mutex_data->mutex...);
    }
  }

  template<C_MutexData FirstData, C_MutexData... MutexData>
  void ExclusiveLock<FirstData, MutexData...>::release()
  {
    locks.reset();
  }

  template<C_MutexData FirstData, C_MutexData... MutexData>
  auto ExclusiveLock<FirstData, MutexData...>::detachFromThread()
  {
    LockPolicy::detachFromThread();
    return locks;
  }

  template<C_MutexHolder FirstHolder, C_MutexHolder... MutexHolder>
  ExclusiveLockGate<FirstHolder, MutexHolder...>::ExclusiveLockGate(
      FirstHolder& first_holder, MutexHolder&... holder)
      : ExclusiveLock<MutexData, TypeTraits::ForEach_t<MutexData, MutexHolder>...>(first_holder.getMutexData(), holder.getMutexData()...)
      , FirstHolder(first_holder)
  {
  }

  template<C_MutexHolder FirstHolder, C_MutexHolder... MutexHolder>
  template<C_MutexHolderWithGates SomeHolder>
  auto& ExclusiveLockGate<FirstHolder, MutexHolder...>::operator[](
      SomeHolder& whichHolder) const
  {
    if (!isLockedExclusively(whichHolder.getMutexData())) {
      throw ExceptionNeedLockToAccessContainer();
    }
    // TODO: check if we actually hold this "whichHolder" before handling a gate.
    auto& gate = whichHolder.getWriterGate(this);
    return *gate;
  }

  template<C_MutexHolder FirstHolder, C_MutexHolder... MutexHolder>
  template<typename>
  auto ExclusiveLockGate<FirstHolder, MutexHolder...>::operator->() const
  {
    if (!isLockedExclusively(firstHolder.getMutexData())) {
      throw ExceptionNeedLockToAccessContainer();
    }
    auto& gate = firstHolder.getWriterGate(this);
    return &*gate;
  }

}  // namespace Threads

#endif
