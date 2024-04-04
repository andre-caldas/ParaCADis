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
    mutable Threads::MutexData defaultMutex;
    Threads::MutexData&        mutex = defaultMutex;
    Struct                     theStruct;

    std::thread::id activeThread;
    std::thread     dedicatedThread;

  public:
    using self_t    = ThreadSafeStruct;
    using record_t = Struct;
    using mutex_data_t = MutexData;

    template<typename... Args>
    ThreadSafeStruct(Args&&... args);

    template<C_MutexHolder MutexHolder, typename... Args>
    ThreadSafeStruct(MutexHolder& holder, Args&&... args);

    virtual ~ThreadSafeStruct();

    using ReaderGate = Threads::LocalReaderGate<&self_t::theStruct>;
    using WriterGate = Threads::LocalWriterGate<&self_t::theStruct>;

    ReaderGate getReaderGate() const noexcept { return {*this}; }
    WriterGate getWriterGate() noexcept { return {*this}; }

    void cancelThreads();

    std::thread& getDedicatedThread();

  public:
    constexpr MutexData& getMutexData() const { return mutex; }
    constexpr operator MutexData&() const { return mutex; }
  };

}  // namespace Threads::SafeStructs

#include "ThreadSafeStruct_inl.h"

#endif
