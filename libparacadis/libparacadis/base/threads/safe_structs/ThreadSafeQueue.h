// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2023 André Caldas <andre.em.caldas@gmail.com>            *
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

#include <libparacadis/base/threads/locks/gates.h>
#include <libparacadis/base/threads/locks/writer_locks.h>

#include <deque>
#include <optional>
#include <semaphore>

namespace Threads::SafeStructs
{
  /**
   * Wraps an std::queue, into a mutex protected structure.
   *
   * @attention This class uses semaphores and the ThreadSafeQueue::pull()
   * call blocks waiting for available data. In particular,
   * no locks can be held when ThreadSafeQueue::pull() is called.
   *
   * @attention There is no need to use gateways.
   */
  template<typename T>
  class ThreadSafeQueue
  {
  private:
    mutable MutexData mutex;

    std::counting_semaphore<> semaphore{0};
    std::deque<T>             theDeque;

  public:
    ThreadSafeQueue() = default;
    ThreadSafeQueue(int mutex_layer) : mutex(mutex_layer) {}

    bool empty() const { return theDeque.empty(); }

    template<typename X>
    void push(X&& item)
    {
      [[maybe_unused]]
      ExclusiveLock l{mutex};
      theDeque.emplace_back(std::forward<X>(item));
      semaphore.release();
    }

    T pull()
    {
      assert(!LockPolicy::hasAnyLock()
             && "This call blocks, you cannot hold locks when you call it.");
      semaphore.acquire();

      [[maybe_unused]]
      ExclusiveLock l{mutex};
      auto result = std::move(theDeque.front());
      theDeque.pop_front();
      return result;
    }

    /**
     * Non-blocking pull operation.
     *
     * When you are repeatedly pulling and doing other things,
     * like in a rendering loop, you can try_pull() and simply
     * give up on failure.
     */
    std::optional<T> try_pull()
    {
      assert(!LockPolicy::hasAnyLock()
             && "This call blocks, you cannot hold locks when you call it.");
      if(!semaphore.try_acquire()) {
        return {};
      }

      [[maybe_unused]]
      ExclusiveLock l{std::try_to_lock, mutex};
      if(l.hasTryLockFailed()) {
        semaphore.release();
        return {};
      }
      auto result = std::move(theDeque.front());
      theDeque.pop_front();
      return result;
    }

    constexpr auto& getMutexLike() const { return mutex; }
  };
}
