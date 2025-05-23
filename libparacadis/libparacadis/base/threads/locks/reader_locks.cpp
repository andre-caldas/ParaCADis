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

#include "reader_locks.h"

namespace Threads
{
  void SharedLock::lock()
  {
    assert(locks.empty() && "Already locked!");

    // We can simply lock, without further worries
    // if we lock the lower layers first.
    std::multimap<int, MutexData*> ordered_mutexes;
    auto& _mutexes = getMutexes();

    for(auto m: _mutexes) {
      ordered_mutexes.emplace(m->layer, m);
    }

    locks.reserve(_mutexes.size());
    for(auto [l, m]: ordered_mutexes) {
      locks.emplace_back(m->mutex);
    }
  }


  bool SharedLock::try_lock()
  {
    assert(locks.empty() && "Already locked!");
    auto& _mutexes = getMutexes();

    locks.reserve(_mutexes.size());
    for(auto m: _mutexes) {
      std::shared_lock temp_lock{m->mutex, std::try_to_lock};
      if(!temp_lock.owns_lock()) {
        release();
        return false;
      }
      locks.emplace_back(std::move(temp_lock));
    }
    return true;
  }

  bool SharedLock::hasTryLockFailed() const
  {
    auto& _mutexes = getMutexes();
    return locks.size() != _mutexes.size();
  }


  void SharedLock::release()
  {
    locks.clear();
    detachFromThread();
  }
}
