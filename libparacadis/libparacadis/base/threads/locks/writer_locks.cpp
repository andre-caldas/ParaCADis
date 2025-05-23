// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2023-2024 André Caldas <andre.em.caldas@gmail.com>       *
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

#include "writer_locks.h"

#include <libparacadis/base/threads/message_queue/MutexSignal.h>

namespace Threads
{
  void ExclusiveLock::lock()
  {
    assert(locks.empty() && "Already locked!");

    // We mimic std::lock, which unfortunately:
    // 1. Demands two mutexes or more.
    // 2. Only works with templates, not with a dynamic set of mutexes.
    auto& _mutexes = getMutexes();
    if(_mutexes.empty()) {
      return;
    }

    locks.reserve(_mutexes.size());

    auto current = _mutexes.begin();
    auto first = _mutexes.begin();
    do
    {
      locks.clear();
      locks.emplace_back(std::unique_lock{(*current)->mutex});
      while(true)
      {
        ++current;
        if(current == _mutexes.end()) {
          current = _mutexes.begin();
        }
        if(current == first) {
          break;
        }
        std::unique_lock next{(*current)->mutex, std::try_to_lock_t{}};
        if(!next) {
          first = current;
          break;
        }
        locks.emplace_back(std::move(next));
      }
    } while(current != first);
    assert(locks.size() == _mutexes.size());
  }

  bool ExclusiveLock::hasTryLockFailed() const
  {
    auto& _mutexes = getMutexes();
    return locks.size() != _mutexes.size();
  }

  bool ExclusiveLock::try_lock()
  {
    assert(locks.empty() && "Already locked!");
    auto& _mutexes = getMutexes();

    locks.reserve(_mutexes.size());
    for(auto m: _mutexes) {
      std::unique_lock temp_lock{m->mutex, std::try_to_lock};
      if(!temp_lock.owns_lock()) {
        release();
        return false;
      }
      locks.emplace_back(std::move(temp_lock));
    }
    return true;
  }

  void ExclusiveLock::release()
  {
    auto& _mutexes = getMutexes();
    for(auto mutex: _mutexes) {
      for(auto signal: mutex->active_signals) {
        signal->emit_signal();
      }
    }
    detachFromThread();
  }

  ExclusiveLock::~ExclusiveLock()
  {
    release();
  }
}
