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
                   message_count_weak = self->messageCount.getWeakPtr()] {
      while(true) {
        auto message_count = message_count_weak.lock();
        if(!message_count) {
          return;
        }
        message_count->acquire();

        std::function<void()> call_back;
        { // shared_ptr lock and mutex lock
          auto self = self_weak.lock();
          if(!self) {
            return;
          }

          WriterGate gate{self->callBacks};
          assert(!gate->empty());
          if(gate->empty()) {
            continue;
          }
          call_back = std::move(gate->front());
          gate->pop_front();
        }

        call_back();
      }
    };

    std::thread{std::move(lambda)}.detach();
  }

  void SignalQueue::push(function_t&& callback)
  {
    WriterGate gate{callBacks};
    gate->emplace_back(std::move(callback));
  }

}
