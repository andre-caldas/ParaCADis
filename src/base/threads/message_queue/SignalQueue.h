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

#ifndef MessageQueue_SignalQueue_H
#define MessageQueue_SignalQueue_H

#include <base/expected_behaviour/SharedPtr.h>
#include <base/threads/safe_structs/ThreadSafeQueue.h>

#include <functional>
#include <memory>
#include <semaphore>

namespace Threads
{

  /**
   * Recieves and executes in order every recieved "message".
   */
  class SignalQueue
  {
    using function_t = std::function<void()>;
    struct record_t { void* id; function_t callback;};

    using queue_t = SafeStructs::ThreadSafeQueue<record_t>;
    /// @attention Not thread safe, while not needed.
    using blocked_t = std::map<void*, std::deque<function_t>>;

  public:
    SignalQueue()
        : callBacks(std::make_shared<queue_t>(MutexData::LOCKFREE))
        , blockedCallBacks(std::make_shared<blocked_t>()) {}

    /**
     * Spawns a thread that continually waits for a signal
     * and executes the corresponding callback.
     *
     * It can be called more than once.
     */
    void run_thread(const SharedPtr<SignalQueue>& self);

    /**
     * Pushes a callback to the queue.
     */
    template<typename T>
    void push(function_t&& callback, T* id)
    { push(std::move(callback), dynamic_cast<void*>(id)); }
    void push(function_t&& callback, void* id);

    template<typename T>
    void block(T* id)
    { block(dynamic_cast<void*>(id)); }
    void block(void* id);

    template<typename T>
    void unblock(T* id)
    { unblock(dynamic_cast<void*>(id)); }
    void unblock(void* id);

  private:
    /**
     * We use a SharedPtr here because we want to make it possible
     * for the SignalQueue be destroyed while we wait for new messages.
     */
    /// @{
    SharedPtr<queue_t> callBacks;
    SharedPtr<blocked_t> blockedCallBacks;
    /// @}
  };

}

#endif
