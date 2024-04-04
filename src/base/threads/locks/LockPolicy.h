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

#ifndef Threads_LockPolicy_H
#define Threads_LockPolicy_H

#include "MutexData.h"
#include "YesItIsAMutex.h"

#include <base/type_traits/Utils.h>

#include <concepts>
#include <memory>
#include <shared_mutex>
#include <type_traits>
#include <unordered_set>

namespace Threads
{

  /**
   * Implements the policy for mutex locking.
   *
   * Implements the following policy: (see README.md)
   * 1. A ExclusiveLock can be instantiated if:
   *    1.1 All mutexes are already locked.
   *    1.2 All mutexes that are not already locked have layer number
   *        greater then those that are already locked.
   *
   * 2. A SharedLock of level (layer number) `n` can be instantiated if:
   *    2.1 The mutex is already locked.
   *    2.2 There is no ExclusiveLock of level `n` or higher.
   *    2.3 There is no SharedLock of level higher than `n`.
   *
   * @see MutexData.
   */
  class LockPolicy
  {
  public:
    static bool hasAnyLock();
    static bool isLocked(const MutexData* mutex);
    static bool isLocked(const MutexData& mutex);
    static bool isLockedExclusively(const MutexData* mutex);
    static bool isLockedExclusively(const MutexData& mutex);

    void detachFromThread();
    void attachToThread(bool is_exclusive);

  protected:
    /**
     * Implements the lock policy.
     * @param is_exclusive - Is it an exclusive lock?
     * @param mutex - Each pair is composed of the mutex to be locked (first)
     * and a mutex that if already locked imposes a new layer for
     * threadMutexesLayers.
     */
    template<C_MutexData... MutN>
    LockPolicy(bool is_exclusive, MutN&... mutex);

    template<typename... MutN>
    LockPolicy(bool is_exclusive, MutN&... mutex);

    LockPolicy() = delete;
    virtual ~LockPolicy();

    bool isDetached() const;
    bool hasIgnoredMutexes() const;

    int minLayerNumber() const;
    int maxLayerNumber() const;

    const std::unordered_set<MutexData*>& getMutexes() const;

  private:
    bool is_detached         = true;
    bool has_ignored_mutexes = false;

    std::unordered_set<MutexData*> mutexes;

    template<C_GatherMutexData G, typename... MutN>
    inline void unfold_mutexes(G& g, MutN&... mutex);

    template<C_MutexData M, typename... MutN>
    inline void unfold_mutexes(M& m, MutN&... mutex);

    template<C_MutexData ... MutN>
    void unfold_mutexes(MutN&... mutex);

    void _processLock(bool is_exclusive);
    void _processExclusiveLock();
    void _processNonExclusiveLock();
    /**
     * @brief Removes information from thread_local variables.
     */
    void _detachFromThread();
  };

}  // namespace Threads

#include "LockPolicy_inl.h"

#endif
