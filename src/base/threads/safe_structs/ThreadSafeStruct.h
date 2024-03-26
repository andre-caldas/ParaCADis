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

#include <base/threads/locks/ReaderLock.h>
#include <base/threads/locks/WriterLock.h>

#include <thread>

namespace Threads::SafeStructs
{

  /**
   * @brief Encapsulates a struct/class to use SharedLock and ExclusiveLock.
   */
  template<typename Struct>
  class ThreadSafeStruct
  {
  public:
    using self_t    = ThreadSafeStruct;
    using record_t = Struct;

    using r_lock_t = ReaderLock<ThreadSafeStruct<Struct>>;
    template<auto LocalPointer>
    using r_lock_local_pointer_t
        = ReaderLock<ThreadSafeStruct<Struct>, LocalPointer>;

    using w_lock_t = WriterLock<ThreadSafeStruct<Struct>>;
    template<auto LocalPointer>
    using w_lock_local_pointer_t
        = WriterLock<ThreadSafeStruct<Struct>, LocalPointer>;

    template<typename... Args>
    ThreadSafeStruct(Args&&... args);

    template<C_MutexHolder MutexHolder, typename... Args>
    ThreadSafeStruct(MutexHolder& holder, Args&&... args);

    virtual ~ThreadSafeStruct();

    [[maybe_unused, nodiscard]] auto lockForReading() const;

    auto continueReading();

    template<auto LocalPointer>
    auto lockPointerForReading() const;

    auto startWriting();

    auto continueWriting();

    template<auto LocalPointer>
    auto lockPointerForWriting();

    struct ReaderGate {
      ReaderGate(const self_t* self);
      const self_t* self;

      const Struct& operator*() const;
      const Struct* operator->() const;
    };

    const ReaderGate& getReaderGate() const noexcept;

    struct WriterGate {
      WriterGate(self_t* self);
      self_t* self;

      Struct& operator*() const;
      Struct* operator->() const;
    };

    const WriterGate& getWriterGate() noexcept;

    void cancelThreads();

    std::thread& getDedicatedThread();

  public:
    // TODO: eliminate this or the gate version.
    constexpr MutexData& getMutexData() const { return mutex; }
    constexpr operator MutexData&() const { return mutex; }

  private:
    const ReaderGate reader_gate{this};
    const WriterGate writer_gate{this};

    mutable Threads::MutexData defaultMutex;
    Threads::MutexData&        mutex = defaultMutex;
    Struct                     theStruct;

    std::thread::id activeThread;
    std::thread     dedicatedThread;
    friend class WriterLock<ThreadSafeStruct<Struct>>;
  };

}  // namespace Threads::SafeStructs

#include "ThreadSafeStruct_inl.h"

#endif
