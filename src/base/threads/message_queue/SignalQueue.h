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
  public:
    SignalQueue()
        : messageCount(std::make_shared<std::counting_semaphore<>>(0)) {}

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
    void push(function_t&& callback);

  private:
    SafeStructs::ThreadSafeQueue<function_t> callBacks;

    /**
     * We use a SharedPtr here because we want to make it possible
     * for the SignalQueue be destroyed while we wait for new messages.
     */
    SharedPtr<std::counting_semaphore<>> messageCount{0};
  };

}

#endif
