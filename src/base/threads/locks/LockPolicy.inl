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

#include "LockPolicy.h"
#include "exceptions.h"

#include <algorithm>
#include <memory>
#include <mutex>
#include <shared_mutex>

namespace Threads
{

  template<IsMutxexPair... MutN>
  LockPolicy::LockPolicy(bool is_exclusive, MutN*... mutex)
      : mutexes{mutex...}
  {
    // Shared locks are acquired one by one.
    assert(!is_esclusive || mutexes.size() <= 1);
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

  template<>
  struct MutexDataPointer<MutexData*> {
    MutexDataPointer(MutexData* mutex)
        : mutex(mutex)
    {
    }

    auto getPair() { return mutex; }

    MutexData* mutex;
  };

  template<>
  struct MutexDataPointer<MutexData> : MutexDataPointer<MutexData*> {
    MutexDataPointer(MutexData& mutex)
        : MutexDataPointer<MutexData*>(&mutex)
    {
    }
  };

  template<IsMutexHolder FirstHolder, IsMutexHolder... MutexHolder>
  ExclusiveLock<FirstHolder, MutexHolder...>::ExclusiveLock(
      FirstHolder& first_holder, MutexHolder&... holder)
      : LockPolicy(
            true, MutexDataPointer{first_holder}.getPair(),
            MutexDataPointer{holder}.getPair()...)
      , FirstHolder(first_holder)
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
        getMutexes().empty()
        || getMutexes().size() == 1 + sizeof...(MutexHolder));
    if (getMutexes().size() == 1 + sizeof...(MutexHolder)) {
      /*
       * It would be more natural if we could pass "mutexes" to the constructor.
       * But there is only the option to list all mutexes at compile time.
       * Fortunately, mutexes = {container.getMutexPtr()...}.
       */
      locks = std::make_shared<locks_t>(
          MutexDataPointer{first_holder}.getPair()->mutex,
          MutexDataPointer{holder}.getPair()->mutex...);
    }
  }

  template<IsMutexHolder FirstHolder, IsMutexHolder... MutexHolder>
  template<IsMutexHolder SomeHolder>
  auto& ExclusiveLock<FirstHolder, MutexHolder...>::operator[](SomeHolder& tsc) const
  {
    if (!isLockedExclusively(tsc.getMutexData())) {
      throw ExceptionNeedLockToAccessContainer();
    }
    auto& gate = tsc.getWriterGate(this);
    return *gate;
  }

  template<IsMutexHolder FirstHolder, IsMutexHolder... MutexHolder>
  template<IsMutexHolder>
  auto ExclusiveLock<FirstHolder, MutexHolder...>::operator->() const
  {
    if (!isLockedExclusively(FirstHolder.getMutexData())) {
      throw ExceptionNeedLockToAccessContainer();
    }
    auto& gate = FirstHolder.getWriterGate(this);
    return &*gate;
  }

  template<IsMutexHolder FirstHolder, IsMutexHolder... MutexHolder>
  void ExclusiveLock<FirstHolder, MutexHolder...>::release()
  {
    locks.reset();
  }

  template<IsMutexHolder FirstHolder, IsMutexHolder... MutexHolder>
  auto ExclusiveLock<FirstHolder, MutexHolder...>::detachFromThread()
  {
    LockPolicy::detachFromThread();
    return locks;
  }

}  // namespace Threads

