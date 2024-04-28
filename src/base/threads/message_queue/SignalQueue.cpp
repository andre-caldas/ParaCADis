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

#include "SignalQueue.h"

#include <base/threads/locks/writer_locks.h>

#include <thread>

namespace Threads
{

  void SignalQueue::run_thread(const SharedPtr<SignalQueue>& self)
  {
    auto lambda = [self_weak = self.getWeakPtr(),
                   call_backs_weak = self->callBacks.getWeakPtr()] {
      while(true) {
        auto call_backs = call_backs_weak.lock();
        if(!call_backs) {
          return;
        }

        auto call_back = call_backs->pull();
        call_back();

        // We do not hold "self" while blocked in `pull()`.
        // Because if we have two consumer threads,
        // we do not them to thik "self" is still being used just because
        // the other consumer thred has a SharedPtr.
        auto self = self_weak.lock();
        if(!self) {
          return;
        }
      }
    };

    std::thread{std::move(lambda)}.detach();
  }

  void SignalQueue::push(function_t&& callback)
  {
    callBacks->push(std::move(callback));
  }

}
