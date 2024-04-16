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

#ifndef BASE_Threads_ThreadSafeStruct_H
#define BASE_Threads_ThreadSafeStruct_H

#include <base/threads/locks/reader_locks.h>
#include <base/threads/locks/writer_locks.h>

#include <thread>

namespace Threads::SafeStructs
{

  /**
   * @brief Encapsulates a struct/class to use SharedLock and ExclusiveLock.
   */
  template<typename Struct>
  class ThreadSafeStruct
  {
  private:
    mutable Threads::MutexData mutex;
    Struct                     theStruct;

    std::thread::id activeThread;
    std::thread     dedicatedThread;

  public:
    using self_t   = ThreadSafeStruct;
    using record_t = Struct;

    template<typename... Args>
    ThreadSafeStruct(Args&&... args);

    virtual ~ThreadSafeStruct();

    using GateInfo = Threads::LocalGateInfo<&self_t::theStruct,
                                            &self_t::mutex>;

    void cancelThreads();

    std::thread& getDedicatedThread();

  public:
    constexpr auto& getMutexLike() const { return mutex; }
  };

}  // namespace Threads::SafeStructs

#include "ThreadSafeStruct_inl.h"

#endif
