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

#ifndef Threads_YesItIsAMutex_H
#define Threads_YesItIsAMutex_H

#include <condition_variable>
#include <mutex>
#include <shared_mutex> // TODO: Remove-me.

namespace Threads
{

using YesItIsAMutex = std::shared_mutex;
#if 0
  /**
   * Like a shared_mutex. But it is not bound to a thread.
   */
  class YesItIsAMutex
  {
  public:
    YesItIsAMutex()                               = default;
    YesItIsAMutex(const YesItIsAMutex&)           = delete;
    YesItIsAMutex operator=(const YesItIsAMutex&) = delete;

    void lock();
    bool try_lock();
    void unlock();

    void lock_shared();
    bool try_lock_shared();
    void unlock_shared();

  private:
    /*
     * This should be implemented with two semaphores: one binary and one counter.
     * But we are still in C++17 in FreeCAD.
     */
    std::mutex pivot;
    int        shared_counter        = 0;
    bool       is_exclusively_locked = false;
    /**
     * Locking consists of trying to lock and waiting for the condition to change.
     */
    std::condition_variable released;
    std::mutex              released_condition_lock;
  };
#endif

}  // namespace Threads

#endif
