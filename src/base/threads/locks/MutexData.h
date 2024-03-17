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

#ifndef Threads_MutexData_H
#define Threads_MutexData_H

#include "YesItIsAMutex.h"

#include <limits>
#include <type_traits>

namespace Threads
{

  /**
   * The lock policy uses a @class MutexData to represent one mutex.
   *
   * The lock policy organizes locks in a hierarchy.
   * In a simple setting, shared locks can be locked
   * as long as the thread does not already hold any exclusive lock.
   * This ensures that shared lock will never dead lock as long as
   * we arrange the exclusive locks to never deadlock.
   * The simplest policy on exclusive locks is to never allow an exclusive
   * lock whanever our thread already holds a lock. In this simple setting,
   * all needed exclusive locks need to be locked at the same time.
   * The policy does not allow you to acquire any lock, exclusive or shared,
   * after acquiring an exclusive lock.
   *
   * However, in order to allow a little more flexibility,
   * the locks can be arranged in a hierarchy. So, the above simple policy
   * is implemented per hierarchy layer. The MutexData class implements
   * this hierarchy by keeping a #layer number.
   */
  struct MutexData {
    YesItIsAMutex mutex;
    const int     layer = 0;

    static constexpr int LOCKFREE = std::numeric_limits<int>::max();

    /// Default #layer and not "lock free".
    MutexData() = default;
    MutexData(int layer) : layer(layer) {}
  };

  template<typename T>
  concept C_MutexData = std::is_same<T, MutexData>::value;

}  // namespace Threads

#endif

