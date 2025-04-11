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
#include <base/threads/utils.h>

#include <thread>

namespace Threads
{
  void SignalQueue::run_thread(const SharedPtr<SignalQueue>& self)
  {
    auto lambda = [self_weak = self.getWeakPtr(),
                   callbacks_weak = self->callBacks.getWeakPtr(),
                   blocked_weak = self->blockedCallBacks.getWeakPtr()] {
      while(true) {
        auto callbacks = callbacks_weak.lock();
        if(!callbacks) {
          return;
        }
        auto blocked = blocked_weak.lock();
        if(!blocked) {
          return;
        }

        auto record = callbacks->pull();
        if(blocked->contains(record.id)) {
          blocked->at(record.id).push_back(std::move(record.callback));
        } else {
          record.callback();
        }

        // We do not hold "self" while blocked in `pull()`.
        // Because if we have two consumer threads,
        // we do not them to think "self" is still being used just because
        // the other consumer thred has a SharedPtr.
        auto self = self_weak.lock();
        if(!self) {
          return;
        }
      }
    };

    std::thread thread{std::move(lambda)};
    Threads::set_thread_name(thread, "signal queue");
    thread.detach();
  }

  void SignalQueue::try_run()
  {
    while(auto record = callBacks->try_pull()) {
      if(blockedCallBacks->contains(record->id)) {
        blockedCallBacks->at(record->id).push_back(std::move(record->callback));
      } else {
        record->callback();
      }
    }
  }

  void SignalQueue::push(function_t&& callback, void* id)
  {
    callBacks->push(record_t{.id=id, .callback=std::move(callback)});
  }

  void SignalQueue::block(void* id)
  {
    /*
     * Everything has to be executed in the queue thread.
     * So, we simply push a blocking lambda to the queue.
     */
    assert(id != nullptr);
    auto lambda = [id, blocked_weak = blockedCallBacks.getWeakPtr()] {
      auto blocked = blocked_weak.lock();
      if(!blocked) { return; }
      assert(!blocked->contains(id) && "Already blocked!");
      /*
       * The mere existance of "id" in blockedCallBacks indicates that
       * all callbacks associated to this "id" should no be executed.
       * The have to be stored inside blocked->at(id).
       */
      blocked->try_emplace(id);
    };
    push(std::move(lambda), nullptr);
  }

  void SignalQueue::unblock(void* id)
  {
    /*
     * Everything has to be executed in the queue thread.
     * So, we simply push an unblocking lambda to the queue.
     */
    assert(id != nullptr);
    auto lambda = [id, blocked_weak = blockedCallBacks.getWeakPtr()] {
      auto blocked = blocked_weak.lock();
      if(!blocked) { return; }
      assert(blocked->contains(id) && "Not blocked!");
      auto nh = blocked->extract(id);
      for(auto& callback: nh.mapped()) {
        callback();
      }
    };
    push(std::move(lambda), nullptr);
  }
}
