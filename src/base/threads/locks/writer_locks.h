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

#include <memory>
#include <vector>

namespace Threads
{
  /**
   * Locks many mutexes exclsively (for writing).
   */
  class ExclusiveLock : public LockPolicy
  {
  public:
    [[nodiscard]]
    ExclusiveLock(ExclusiveLock&& other_lock) = default;
    template<C_MutexLike ...Mutex>
    [[nodiscard]]
    ExclusiveLock(Mutex&... mutex);

    /**
     * Prematurely releases the lock.
     *
     * Usually, we do not want to bother about releasing the lock,
     * because we use RAII. However, sometimes you want to construct
     * a structure while holding the lock and have it to remain valid
     * after the lock was released.
     *
     * @attention
     * Selfom use this. Rethink your design... you probably ain't gonna need it.
     */
    void release();
    ~ExclusiveLock();

  private:
    std::vector<std::unique_lock<YesItIsAMutex>> locks;
    void lock();
  };


  /**
   * The WriterGate class for many (or zero!) data.
   */
  template<C_MutexHolderWithGates ... Holders>
  class WriterGate
  {
  public:
    WriterGate(Holders&... holders);

    template<C_MutexHolderWithGates Holder>
    auto& operator[](Holder& holder) const;

    /**
     * Prematurely releases the lock.
     *
     * Usually, we do not want to bother about releasing the lock,
     * because we use RAII. However, sometimes you want to construct
     * a structure while holding the lock and have it to remain valid
     * after the lock was released.
     *
     * @attention
     * Selfom use this. Rethink your design... you probably ain't gonna need it.
     */
    void release();

  private:
    ExclusiveLock lock;
#ifndef NDEBUG
    bool released = false;
    const std::unordered_set<const void*> all_holders;
#endif
  };


  /**
   * The WriterGate class for one data.
   */
  template<C_MutexHolderWithGates Holder>
  class WriterGate<Holder>
  {
  public:
    WriterGate(Holder& holder);

    auto& operator*();
    auto* operator->() { return &**this; }

    /**
     * Prematurely releases the lock.
     *
     * Usually, we do not want to bother about releasing the lock,
     * because we use RAII. However, sometimes you want to construct
     * a structure while holding the lock and have it to remain valid
     * after the lock was released.
     *
     * @attention
     * Selfom use this. Rethink your design... you probably ain't gonna need it.
     */
    void release();

  private:
    ExclusiveLock lock;
    Holder& holder;
#ifndef NDEBUG
    bool released = false;
#endif
  };
}  // namespace Threads

#include "writer_locks.hpp"
