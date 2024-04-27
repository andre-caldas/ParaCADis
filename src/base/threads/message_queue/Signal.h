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

#ifndef MessageQueue_Signal_H
#define MessageQueue_Signal_H

#include <base/expected_behaviour/SharedPtr.h>
#include <base/threads/safe_structs/ThreadSafeMap.h>

#include <atomic>
#include <functional>

namespace Threads
{

  class SignalQueue;

  /**
   * A Signal that can be emited to inform "connected"
   * objects about events.
   *
   * In practice, it holds a list of callbacks that have to be called
   * whenever the event is emited.
   *
   * The emitting side does not wait for the calls to be made.
   * The callbacks are executed in other threads by the corresponding SignalQueue.
   */
  template<typename... Args>
  class Signal
  {
  public:
    /**
     * Sends the signal to all registered callbacks.
     */
    void emit_signal(Args... args);

    /**
     * Connects the signal to a callback (to->*member)
     * through the given @a queue.
     */
    template<class T, std::derived_from<T> SignalFrom, class SignalTo>
    int connect(const SharedPtr<SignalFrom>& from,
                const SharedPtr<SignalQueue>& queue,
                const SharedPtr<SignalTo>& to,
                void (SignalTo::*member)(SharedPtr<T>, Args...));

    void disconnect(int id);

  private:
    std::atomic<int> id{0};
    struct Data {
      WeakPtr<SignalQueue> queue_weak;
      WeakPtr<void> to_void_weak;  // Just to auto disconnect.
      std::function<void(Args... args)> call_back;
    };

    SafeStructs::ThreadSafeUnorderedMap<int, Data> callBacks;
  };

}

#include "Signal_impl.h"

#endif
