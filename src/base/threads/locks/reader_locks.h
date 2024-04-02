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

#ifndef Threads_ReaderLocks_H
#define Threads_ReaderLocks_H

#include "gates.h"
#include "LockPolicy.h"
#include "YesItIsAMutex.h"

#include <base/expected_behaviour/SharedPtr.h>

#include <shared_mutex>
#include <vector>

namespace Threads
{

  class SharedLock : public LockPolicy
  {
  public:
    /**
     * The movable constructor is so that you can have the mutex locked
     * and handled to a LockedIterator.
     */
    [[nodiscard]]
    SharedLock(SharedLock&& other_lock) = default;
    template<C_MutexGatherOrData Mutex>
    [[nodiscard]]
    SharedLock(Mutex& mutex);
    template<C_MutexHolder Holder>
    [[nodiscard]]
    SharedLock(const Holder& holder) : SharedLock(holder.getMutexData())
    {
    }

  private:
    std::vector<std::shared_lock<YesItIsAMutex>> locks;
  };


  /**
   * The ReaderGate class.
   */
  /// @{
  /// Allow template specialization.
  template<auto PTR_TO_MEMBER>
  class ReaderGate {};

  template<C_MutexHolder Holder, typename T, T Holder::* localData>
  class ReaderGate<localData> : public _GateBase<SharedLock>
  {
  public:
    ReaderGate(const Holder& holder);

    const T& operator*() const;
    const T* operator->() const;

  private:
    const T* data;
  };
  /// @}

}  // namespace Threads

#include "reader_locks_impl.h"

#endif
