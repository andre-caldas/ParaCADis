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

#pragma once

#include <base/threads/locks/reader_locks.h>
#include <base/threads/locks/writer_locks.h>

#include <thread>
#include <utility>

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

  public:
    using self_t   = ThreadSafeStruct;
    using record_t = Struct;

    ThreadSafeStruct() = default;
    ThreadSafeStruct(MutexLayer layer);
    ThreadSafeStruct(record_t&& record);
    ThreadSafeStruct(MutexLayer layer, record_t&& record);

    /**
     * Move constructors.
     * @attention We assume without check that no other threads have
     * access to the moved structure.
     */
    ThreadSafeStruct(ThreadSafeStruct&& other)
        : mutex(other.mutex.layer)
        , theStruct(std::move(other.theStruct))
    {}

    template<typename... T>
    ThreadSafeStruct(T&&... t);

    template<typename... T>
    ThreadSafeStruct(MutexLayer layer, T&&... t);


    // We could have a copy constructor.
    // But... do we want to?
//    ThreadSafeStruct(const ThreadSafeStruct& other);

    virtual ~ThreadSafeStruct() = default;

    using GateInfo = Threads::LocalGateInfo<&self_t::theStruct,
                                            &self_t::mutex>;

    constexpr auto& getMutexLike() const { return mutex; }

    /**
     * Direct access to the (un)protected structure.
     *
     * This can be used when you are totally sure the object is not shared, yet.
     * For example, during construction.
     */
    constexpr Struct& _unsafeStructAccess() { return theStruct; }
  };
}

#include "ThreadSafeStruct.hpp"
