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
#include <stack>
#include <unordered_set>

namespace Threads
{

  thread_local std::unordered_set<const MutexData*> threadExclusiveMutexes;
  thread_local std::unordered_set<const MutexData*> threadNonExclusiveMutexes;
  thread_local std::stack<bool>                     isLayerExclusive;
  thread_local std::stack<int>                      layerNumber;
  thread_local std::stack<std::unordered_set<const MutexData*>> threadMutexLayers;

  bool LockPolicy::hasAnyLock()
  {
    assert(
        threadMutexLayers.empty()
        == (threadExclusiveMutexes.empty() && threadNonExclusiveMutexes.empty()));
    return !threadMutexLayers.empty();
  }

  bool LockPolicy::isLocked(const MutexData* mutex)
  {
    bool has_exclusive     = threadExclusiveMutexes.contains(mutex);
    bool has_non_exclusive = threadNonExclusiveMutexes.contains(mutex);
    return has_exclusive || has_non_exclusive;
  }

  bool LockPolicy::isLocked(const MutexData& mutex)
  {
    return isLocked(&mutex);
  }

  bool LockPolicy::isLockedExclusively(const MutexData* mutex)
  {
    return threadExclusiveMutexes.contains(mutex);
  }

  bool LockPolicy::isLockedExclusively(const MutexData& mutex)
  {
    return isLockedExclusively(&mutex);
  }

  void LockPolicy::detachFromThread()
  {
    if (has_ignored_mutexes) {
      assert(false);
      throw Exception::NewThreadRequiresReleaseableLock{};
    }
    assert(!mutexes.empty());

    // Confirm all mutexes are in the topmost layer.
    assert(!threadMutexLayers.empty());
    auto layer = threadMutexLayers.top();
    for (auto mutex: mutexes) {
      if (!layer.contains(mutex)) {
        assert(false);
        throw Exception::NewThreadRequiresReleaseableLock{};
      }
    }

    _detachFromThread();
  }

  bool LockPolicy::isDetached() const
  {
    return is_detached;
  }

  bool LockPolicy::hasIgnoredMutexes() const
  {
    return has_ignored_mutexes;
  }

  int LockPolicy::minLayerNumber() const
  {
    return std::transform_reduce(
        mutexes.cbegin(), mutexes.cend(), std::numeric_limits<int>::max(),
        [](int a, int b){return std::min(a,b);},
        [](const MutexData* m) { return m->layer; });
  }

  int LockPolicy::maxLayerNumber() const
  {
    return std::transform_reduce(
        mutexes.cbegin(), mutexes.cend(), std::numeric_limits<int>::min(),
        [](int a, int b){return std::max(a,b);},
        [](const MutexData* m) { return m->layer; });
  }

  const std::unordered_set<const MutexData*>& LockPolicy::getMutexes() const
  {
    return mutexes;
  }

  void LockPolicy::_detachFromThread()
  {
    if (is_detached) {
      return;
    }
    is_detached = true;

    if (mutexes.empty()) {
      return;
    }

    for (auto mutex: mutexes) {
      assert(
          threadExclusiveMutexes.contains(mutex)
          || threadNonExclusiveMutexes.contains(mutex));
      threadExclusiveMutexes.erase(mutex);
      threadNonExclusiveMutexes.erase(mutex);
      assert(!threadMutexLayers.empty());
      threadMutexLayers.top().erase(mutex);
    }

    assert(!threadMutexLayers.empty());
    if (!threadMutexLayers.empty() && threadMutexLayers.top().empty()) {
      threadMutexLayers.pop();
      isLayerExclusive.pop();
      layerNumber.pop();
      assert(isLayerExclusive.size() == threadMutexLayers.size());
      assert(isLayerExclusive.size() == layerNumber.size());
    }
  }

  void LockPolicy::attachToThread(bool is_exclusive)
  {
    assert(!hasAnyLock());
    assert(!mutexes.empty());
    assert(!has_ignored_mutexes);
    if (!is_detached) {
      assert(false);
      throw Exception::NewThreadRequiresMovedLock{};
    }
    _processLock(is_exclusive);
    assert(
        mutexes.size() == threadExclusiveMutexes.size()
        || mutexes.size() == threadNonExclusiveMutexes.size());
  }

  void LockPolicy::_processLock(bool is_exclusive)
  {
    assert(is_detached);
    is_detached = false;

    if (mutexes.empty()) {
      // A container.end() does this!
      return;
    }

    /*
     * When we have absolutely no mutexes,
     * just lock as required.
     */
    if (!hasAnyLock()) {
      assert(threadExclusiveMutexes.empty());
      assert(threadNonExclusiveMutexes.empty());

      // Lock everything that was requested.
      threadMutexLayers.push(mutexes);
      assert(threadMutexLayers.size() == 1);
      isLayerExclusive.push(is_exclusive);
      layerNumber.push(maxLayerNumber());
      assert(isLayerExclusive.size() == threadMutexLayers.size());
      assert(isLayerExclusive.size() == layerNumber.size());
      if (is_exclusive) {
        threadExclusiveMutexes = mutexes;
      } else {
        threadNonExclusiveMutexes = mutexes;
      }
      return;
    }

    if (is_exclusive) {
      _processExclusiveLock();
    } else {
      _processNonExclusiveLock();
    }
  }

  void LockPolicy::_processExclusiveLock()
  {
    assert(!threadExclusiveMutexes.empty() || !threadNonExclusiveMutexes.empty());
    assert(!threadMutexLayers.empty());
    assert(isLayerExclusive.size() == threadMutexLayers.size());
    assert(isLayerExclusive.size() == layerNumber.size());

    // remove already locked "exclusive" mutexes from the list.
    for (auto it = mutexes.begin(); it != mutexes.end();) {
      if (threadExclusiveMutexes.contains(*it)) {
        assert(isLockedExclusively(*it));
        it                  = mutexes.erase(it);
        has_ignored_mutexes = true;
      } else {
        ++it;
      }
    }

    if (mutexes.empty()) {
      // Nothing to lock.
      assert(!threadMutexLayers.top().empty());
      return;
    }

    // do not allow already locked "non-exclusive" mutexes in the list.
    for (auto mutex: mutexes) {
      if (threadNonExclusiveMutexes.contains(mutex)) {
        // We cannot exclusively lock if it is already shared.
        throw Exception::NoExclusiveOverNonExclusive();
      }
    }

    // Create layers if needed.
    int min = minLayerNumber();
    assert(!layerNumber.empty());
    if (min <= layerNumber.top()) {
      throw Exception::AlreadyHasLayer{min};
    }

    /// @todo search those inserts and substitute for insert_range().
    threadExclusiveMutexes.insert(mutexes.cbegin(), mutexes.cend());
    threadMutexLayers.push(mutexes);
    isLayerExclusive.push(true);
    layerNumber.push(maxLayerNumber());
    assert(isLayerExclusive.size() == threadMutexLayers.size());
    assert(isLayerExclusive.size() == layerNumber.size());
    assert(!threadMutexLayers.top().empty());
    assert(!threadExclusiveMutexes.empty());
  }

  void LockPolicy::_processNonExclusiveLock()
  {
    assert(!threadExclusiveMutexes.empty() || !threadNonExclusiveMutexes.empty());
    assert(!threadMutexLayers.empty());
    assert(isLayerExclusive.size() == threadMutexLayers.size());
    assert(isLayerExclusive.size() == layerNumber.size());

    // remove already locked "exclusive" and "non-exclusive" mutexes from the list.
    for (auto it = mutexes.begin(); it != mutexes.end();) {
      if (threadExclusiveMutexes.contains(*it)
          || threadNonExclusiveMutexes.contains(*it)) {
        assert(isLocked(*it));
        it                  = mutexes.erase(it);
        has_ignored_mutexes = true;
      } else {
        ++it;
      }
    }

    if (mutexes.empty()) {
      // Nothing to lock.
      assert(!threadMutexLayers.top().empty());
      return;
    }

    assert(!isLayerExclusive.empty());
    if (isLayerExclusive.top()) {
      // Create layers if needed.
      int min = minLayerNumber();
      assert(!layerNumber.empty());
      if (min < layerNumber.top()) {
        throw Exception::AlreadyHasBiggerLayer(min);
      }
      if (min == layerNumber.top() && isLayerExclusive.top()) {
        throw Exception::NoLocksAfterExclusiveLock();
      }
      threadMutexLayers.emplace();
      isLayerExclusive.push(false);
      layerNumber.push(maxLayerNumber());
      assert(isLayerExclusive.size() == threadMutexLayers.size());
      assert(isLayerExclusive.size() == layerNumber.size());
    }

    threadMutexLayers.top().insert(mutexes.cbegin(), mutexes.cend());
    threadNonExclusiveMutexes.insert(mutexes.cbegin(), mutexes.cend());
  }

  LockPolicy::~LockPolicy()
  {
    _detachFromThread();
  }

  SharedLock::SharedLock(MutexData& mutex)
      : LockPolicy(false, mutex)
  {
    if (!getMutexes().empty()) {
      assert(getMutexes().size() == 1);
      assert(getMutexes().contains(&mutex));
      lock = std::shared_lock(mutex.mutex);
    }
  }

}  // namespace Threads
