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

#pragma once

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
    [[nodiscard]]
    SharedLock(SharedLock&& other_lock) = default;
    template<C_MutexLike... Mutex>
    [[nodiscard]]
    SharedLock(Mutex&... mutex);

    /**
     * Prematurely releases the lock.
     *
     * Usually, we do not want to bother about releasing the lock,
     * because we use RAII. However, sometimes you want to construct
     * a structure while holding the lock and have it to remain valid
     * after the lock was released.
     *
     * @attention
     * Seldom use this. Rethink your design... you probably ain't gonna need it.
     */
    void release();

  private:
    std::vector<std::shared_lock<YesItIsAMutex>> locks;
    void lock();
  };


  /**
   * The ReaderGate class for many (or zero!) data.
   */
  template<C_MutexHolderWithGates... Holders>
  class ReaderGate
  {
  public:
    ReaderGate(const Holders&... holders);

    template<C_MutexHolderWithGates Holder>
    auto& operator[](const Holder& holder) const;

    /**
     * Prematurely releases the lock.
     *
     * Usually, we do not want to bother about releasing the lock,
     * because we use RAII. However, sometimes you want to construct
     * a structure while holding the lock and have it to remain valid
     * after the lock was released.
     *
     * @attention
     * Seldom use this. Rethink your design... you probably ain't gonna need it.
     */
    void release();

    /**
     * Gets a non-const reference.
     *
     * A big dilema... we do not want `operator*()` to return non-const.
     * However, there are situations where you want a shared lock,
     * because you do not want to change the data, while you sill
     * want a non-const pointer. Maybe you want to store it,
     * like in case of IExport<T>::resolve().
     */
    template<C_MutexHolderWithGates Holder>
    auto& getNonConst(Holder& holder) const;

  private:
    SharedLock lock;
#ifndef NDEBUG
    bool released = false;
    const std::unordered_set<const void*> all_holders;
#endif
  };


  /**
   * The ReaderGate class for one data.
   */
  template<C_MutexHolderWithGates Holder>
  class ReaderGate<Holder>
  {
  public:
    ReaderGate(const Holder& holder);
    ReaderGate(Holder&& holder) = delete;

    const auto& operator*() const;
    const auto* operator->() const { return &**this; }

    /**
     * Prematurely releases the lock.
     *
     * Usually, we do not want to bother about releasing the lock,
     * because we use RAII. However, sometimes you want to construct
     * a structure while holding the lock and have it to remain valid
     * after the lock was released.
     *
     * @attention
     * Seldom use this. Rethink your design... you probably ain't gonna need it.
     */
    void release();

    /**
     * Gets a non-const reference.
     *
     * A big dilema... we do not want `operator*()` to return non-const.
     * However, there are situations where you want a shared lock,
     * because you do not want to change the data, while you sill
     * want a non-const pointer. Maybe you want to store it,
     * like in case of IExport<T>::resolve().
     */
    auto& getNonConst(Holder& holder) const;

  private:
    SharedLock lock;
    const Holder& holder;
#ifndef NDEBUG
    bool released = false;
#endif
  };


  /**
   * Like a ReaderGate class for one data,
   * but it also keeps a private Holder.
   *
   * This is used for cases when you want a ReaderGate
   * for a temporary object or any object that might get
   * destructed while we are using it.
   *
   * It is used in "unit tests" and python bindings.
   */
  template<C_MutexHolderWithGates Holder>
  class ReaderGateKeeper
  {
  public:
    ReaderGateKeeper(Holder holder);

    const auto& operator*() const;
    const auto* operator->() const { return &**this; }

    /**
     * Prematurelly releases the gate.
     */
    void release();

    const Holder& getHolder() const {return holder;}

  private:
    SharedLock lock;
    Holder holder;
    bool released = false;
  };
  /// @}
}  // namespace Threads

#include "reader_locks.hpp"
