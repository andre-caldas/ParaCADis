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

#include <array>
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

  private:
    std::vector<std::shared_lock<YesItIsAMutex>> locks;
  };


  /**
   * The ReaderGate class.
   */
  /// @{
  template<C_MutexHolderWithGates... Holders>
  class ReaderGate
  {
  public:
    ReaderGate(const Holders&... holders);

    template<C_MutexHolderWithGates Holder>
    auto& operator[](const Holder& holder) const;

  private:
    std::array<SharedLock, sizeof...(Holders)> locks;
#ifndef NDEBUG
    const std::unordered_set<const void*> all_holders;
#endif
  };

  template<C_MutexHolderWithGates Holder>
  class ReaderGate<Holder> : public SharedLock
  {
  public:
    ReaderGate(const Holder& holder);

    auto& operator*() { return data; }
    auto* operator->() { return &data; }

  private:
    Holder::ReaderGate::protected_data_t& data;
  };
  /// @}


  /**
   * The LocalReaderGate class.
   */
  /// @{
  /// Allow template specialization.
  template<auto PTR_TO_MEMBER>
  class LocalReaderGate {};

  template<C_MutexHolder Holder, typename T, T Holder::* localData>
  class LocalReaderGate<localData> : public _GateBase<SharedLock>
  {
  public:
    LocalReaderGate(const Holder& holder);

    using protected_data_t = const T;

    const T& operator*() const;
    const T* operator->() const;

  private:
    const T& data;

    static const T& getProtectedData(const Holder& holder) { return holder.*localData; }

    template<C_MutexHolderWithGates... Holders>
    friend class ReaderGate;
  };
  /// @}

}  // namespace Threads

#include "reader_locks_impl.h"

#endif
