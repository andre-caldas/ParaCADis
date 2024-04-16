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

#include "LockPolicy.h"
#include "exceptions.h"

#include <algorithm>
#include <limits>
#include <numeric>
#include <ranges>
#include <stack>
#include <unordered_set>

namespace Threads
{

  struct OperationInfo
  {
    const int max;
    const bool isExclusive;
#ifndef NDEBUG
    std::unordered_set<MutexData*> mutexes;
#endif
  };

  thread_local std::unordered_set<const MutexData*> threadExclusiveMutexes;
  thread_local std::unordered_set<const MutexData*> threadSharedMutexes;
  thread_local std::stack<OperationInfo> operationInfo;

  bool LockPolicy::hasAnyLock()
  {
    assert(
        operationInfo.empty()
        == (threadExclusiveMutexes.empty() && threadSharedMutexes.empty()));
    return !operationInfo.empty();
  }

  bool LockPolicy::isLocked(const MutexData* mutex)
  {
    bool has_shared = threadSharedMutexes.contains(mutex);
    return has_shared || isLockedExclusively(mutex);
  }

  bool LockPolicy::isLocked(const MutexData& mutex)
  {
    return isLocked(&mutex);
  }

  bool LockPolicy::isLocked(const MutexVector& mutexes)
  {
    return std::all_of(mutexes.cbegin(), mutexes.cend(),
                       [](auto m){return LockPolicy::isLocked(*m);});
  }

  bool LockPolicy::isLockedExclusively(const MutexData* mutex)
  {
    return threadExclusiveMutexes.contains(mutex);
  }

  bool LockPolicy::isLockedExclusively(const MutexData& mutex)
  {
    return isLockedExclusively(&mutex);
  }

  int LockPolicy::minMutex() const
  {
    return std::transform_reduce(
        mutexes.cbegin(), mutexes.cend(), std::numeric_limits<int>::max(),
        [](int a, int b){return std::min(a,b);},
        [](const MutexData* m) { return m->layer; });
  }

  int LockPolicy::maxMutex() const
  {
    return std::transform_reduce(
        mutexes.cbegin(), mutexes.cend(), std::numeric_limits<int>::min(),
        [](int a, int b){return std::max(a,b);},
        [](const MutexData* m) { return m->layer; });
  }

  const std::unordered_set<MutexData*>& LockPolicy::getMutexes() const
  {
    return mutexes;
  }

  void LockPolicy::_detachFromThread()
  {
    if (mutexes.empty()) {
      return;
    }

    assert(!operationInfo.empty());
    bool is_exclusive = operationInfo.top().isExclusive;

#ifndef NDEBUG
    assert(operationInfo.top().mutexes == mutexes &&
           "Releases must be in the reverse order of acquisition!");
#endif

    for (auto mutex: mutexes) {
      if(is_exclusive) {
        assert(threadExclusiveMutexes.contains(mutex));
        threadExclusiveMutexes.erase(mutex);
      } else {
        assert(threadSharedMutexes.contains(mutex));
        threadSharedMutexes.erase(mutex);
      }
    }

    operationInfo.pop();
  }

  void LockPolicy::_processLock(bool is_exclusive)
  {
    if (mutexes.empty()) {
      return;
    }

    if (is_exclusive) {
      _processExclusiveLock();
    } else {
      _processSharedLock();
    }
  }

  void LockPolicy::_processExclusiveLock()
  {
    // remove already locked "main" mutexes from the list.
    // And ensures they are not locked non-exclusively.
    for (auto it = mutexes.begin(); it != mutexes.end();) {
      if (threadSharedMutexes.contains(*it)) {
        assert(!threadExclusiveMutexes.contains(*it));
        throw Exception::NoExclusiveOverNonExclusive();
      }
      if (threadExclusiveMutexes.contains(*it)) {
        assert(isLockedExclusively(*it));
        it = mutexes.erase(it);
      } else {
        ++it;
      }
    }

    if (mutexes.empty()) {
      return;
    }

    auto maxM = maxMutex();
    if(!operationInfo.empty()) {
      assert(!threadExclusiveMutexes.empty()
             || !threadSharedMutexes.empty());

      // Check if operations are, according to the policy,
      // compatible with current state.
      auto& current_info = operationInfo.top();
      // New exclusive mutexes must be in a higher level.
      if(minMutex() <= current_info.max) {
        throw Exception::AlreadyHasLayer(minMutex());
      }
    }

    // Store OperationInfo.
    operationInfo.emplace(
#ifndef NDEBUG
        OperationInfo{.max=maxM, .isExclusive=true, .mutexes=mutexes}
#else
        OperationInfo{.max=maxM, .isExclusive=true}
#endif
    );

    threadExclusiveMutexes.insert(mutexes.cbegin(), mutexes.cend());
    assert(!threadExclusiveMutexes.empty());
  }

  void LockPolicy::_processSharedLock()
  {
    // remove already locked "main" mutexes from the list.
    // And ensures they are not locked non-exclusively.
    for (auto it = mutexes.begin(); it != mutexes.end();) {
      if (threadExclusiveMutexes.contains(*it)
          || threadSharedMutexes.contains(*it)) {
        assert(isLocked(*it));
        it = mutexes.erase(it);
      } else {
        ++it;
      }
    }

    if (mutexes.empty()) {
      return;
    }

    auto maxM = maxMutex();
    if(!operationInfo.empty()) {
      // Check if operations are, according to the policy,
      // compatible with current state.
      auto& current_info = operationInfo.top();
      if(current_info.isExclusive) {
        // New mutexes need to be in a higher layer.
        if(minMutex() <= current_info.max) {
          throw Exception::NoLocksAfterExclusiveLock();
        }
      } else {
        // New mutexes must be the same level or higher.
        if(minMutex() < current_info.max) {
          throw Exception::AlreadyHasBiggerLayer(minMutex());
        }
      }
    }

    // Store OperationInfo.
    operationInfo.emplace(
#ifndef NDEBUG
        OperationInfo{.max=maxM, .isExclusive=false, .mutexes=mutexes}
#else
        OperationInfo{.max=maxM, .isExclusive=false}
#endif
    );

    threadSharedMutexes.insert(mutexes.cbegin(), mutexes.cend());
    assert(!threadSharedMutexes.empty());
  }

  LockPolicy::~LockPolicy()
  {
    _detachFromThread();
  }

}  // namespace Threads
