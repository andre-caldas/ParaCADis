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
    const int maxPivot;
    const int max;
    const bool isExclusive;
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

  bool LockPolicy::isLockedExclusively(const MutexData* mutex)
  {
    if(mutex->my_pivot) {
      return isLockedExclusively(mutex->my_pivot);
    }
    return threadExclusiveMutexes.contains(mutex);
  }

  bool LockPolicy::isLockedExclusively(const MutexData& mutex)
  {
    return isLockedExclusively(&mutex);
  }

  int LockPolicy::minMutex() const
  {
    return std::transform_reduce(
        mainMutexes.cbegin(), mainMutexes.cend(), std::numeric_limits<int>::max(),
        [](int a, int b){return std::min(a,b);},
        [](const MutexData* m) { return m->layer; });
  }

  int LockPolicy::maxMutex() const
  {
    return std::transform_reduce(
        mainMutexes.cbegin(), mainMutexes.cend(), std::numeric_limits<int>::min(),
        [](int a, int b){return std::max(a,b);},
        [](const MutexData* m) { return m->layer; });
  }

  int LockPolicy::minPivot() const
  {
    return std::transform_reduce(
        pivotMutexes.cbegin(), pivotMutexes.cend(), std::numeric_limits<int>::max(),
        [](int a, int b){return std::min(a,b);},
        [](const MutexData* m) { return m->pivotLayer(); });
  }

  int LockPolicy::maxPivot() const
  {
    return std::transform_reduce(
        pivotMutexes.cbegin(), pivotMutexes.cend(), std::numeric_limits<int>::min(),
        [](int a, int b){return std::max(a,b);},
        [](const MutexData* m) { return m->pivotLayer(); });
  }

  const std::unordered_set<MutexData*>& LockPolicy::getMainMutexes() const
  {
    return mainMutexes;
  }

  const std::unordered_set<MutexData*>& LockPolicy::getPivotMutexes() const
  {
    return pivotMutexes;
  }

  void LockPolicy::_detachFromThread()
  {
    if (mainMutexes.empty()) {
      assert(pivotMutexes.empty());
      return;
    }

    assert(!operationInfo.empty());
    bool is_exclusive = operationInfo.top().isExclusive;

    for (auto mutex: mainMutexes) {
      if(is_exclusive) {
        assert(threadExclusiveMutexes.contains(mutex));
        threadExclusiveMutexes.erase(mutex);
      } else {
        assert(threadSharedMutexes.contains(mutex));
        threadSharedMutexes.erase(mutex);
      }
    }

    for (auto mutex: pivotMutexes) {
      assert(threadSharedMutexes.contains(mutex));
      threadSharedMutexes.erase(mutex);
    }

    operationInfo.pop();
  }

  void LockPolicy::_processLock(bool is_exclusive)
  {
    assert(pivotMutexes.empty());
    if (mainMutexes.empty()) {
      assert(false);
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
    assert(pivotMutexes.empty());

    // remove already locked "main" mutexes from the list.
    // And ensures they are not locked non-exclusively.
    bool removed_exclusive = false;
    for (auto it = mainMutexes.begin(); it != mainMutexes.end();) {
      if (threadSharedMutexes.contains(*it)) {
        assert(!threadExclusiveMutexes.contains(*it));
        throw Exception::NoExclusiveOverNonExclusive();
      }
      if (threadExclusiveMutexes.contains(*it)) {
        removed_exclusive = true;
        assert(isLockedExclusively(*it));
        it = mainMutexes.erase(it);
      } else {
        ++it;
      }
    }

    // Either we have already locked every mutex,
    // or we have locked none of them.
    assert(!removed_exclusive || mainMutexes.empty());

    if (mainMutexes.empty()) {
      return;
    }

    _registerPivots();

    auto maxP = maxPivot();
    auto maxM = maxMutex();
    assert(maxP <= maxM);
    if(!operationInfo.empty()) {
      // Check if operations are, according to the policy,
      // compatible with current state.
      auto& current_info = operationInfo.top();

      if(current_info.isExclusive) {
        if(minPivot() <= current_info.max) {
          throw Exception::NoLocksAfterExclusiveLock();
        }
      } else {
        assert(minPivot() <= minMutex());
        // New pivots must be the same level or higher.
        if(minPivot() < current_info.maxPivot) {
          throw Exception::AlreadyHasBiggerLayer(minPivot());
        }
        if(minMutex() <= current_info.max) {
          throw Exception::AlreadyHasLayer(minMutex());
        }
      }
    }

    // Store OperationInfo.
    operationInfo.emplace(
        OperationInfo{.maxPivot=maxP, .max=maxM, .isExclusive=true});

    threadExclusiveMutexes.insert(mainMutexes.cbegin(), mainMutexes.cend());
    threadSharedMutexes.insert(pivotMutexes.cbegin(), pivotMutexes.cend());
    assert(!threadExclusiveMutexes.empty());
  }

  void LockPolicy::_processSharedLock()
  {
    assert(pivotMutexes.empty());

    // remove already locked "main" mutexes from the list.
    // And ensures they are not locked non-exclusively.
    for (auto it = mainMutexes.begin(); it != mainMutexes.end();) {
      if (threadExclusiveMutexes.contains(*it)
          || threadSharedMutexes.contains(*it)) {
        assert(isLocked(*it));
        it = mainMutexes.erase(it);
      } else {
        ++it;
      }
    }

    if (mainMutexes.empty()) {
      return;
    }

    _registerPivots();

    auto maxP = maxPivot();
    auto maxM = maxMutex();
    assert(maxP <= maxM);
    if(!operationInfo.empty()) {
      // Check if operations are, according to the policy,
      // compatible with current state.
      auto& current_info = operationInfo.top();
      if(current_info.isExclusive) {
        if(minPivot() < current_info.max) {
          throw Exception::NoLocksAfterExclusiveLock();
        }
      } else {
        // New pivots must be the same level or higher.
        if(minPivot() < current_info.maxPivot) {
          throw Exception::AlreadyHasBiggerLayer(minPivot());
        }
        if(maxP < current_info.maxPivot) {maxP = current_info.maxPivot;}
        if(maxM < current_info.max) {maxM = current_info.max;}
      }
    }

    // Store OperationInfo.
    operationInfo.emplace(
        OperationInfo{.maxPivot=maxPivot(), .max=maxMutex(), .isExclusive=false});

    threadSharedMutexes.insert(mainMutexes.cbegin(), mainMutexes.cend());
    threadSharedMutexes.insert(pivotMutexes.cbegin(), pivotMutexes.cend());
    assert(!threadSharedMutexes.empty());
  }

  void LockPolicy::_registerPivots()
  {
    for(const auto* mutex: mainMutexes) {
      // The pivot is registered, unless it is also a main mutex.
      while(mutex->my_pivot) {
        if(!mainMutexes.contains(mutex->my_pivot)
            && !threadExclusiveMutexes.contains(mutex->my_pivot)
            && !threadSharedMutexes.contains(mutex->my_pivot))
        {
          pivotMutexes.insert(mutex->my_pivot);
        }
        mutex = mutex->my_pivot;
      }
    }

    assert(std::ranges::none_of(pivotMutexes,
                                [this](auto* mutex)
                                {return mainMutexes.contains(mutex);}));

    assert(std::ranges::none_of(pivotMutexes,
                                [](auto* mutex)
                                {return threadExclusiveMutexes.contains(mutex);}));

    assert(std::ranges::none_of(pivotMutexes,
                                [](auto* mutex)
                                {return threadSharedMutexes.contains(mutex);}));
  }

  LockPolicy::~LockPolicy()
  {
    _detachFromThread();
  }

}  // namespace Threads
