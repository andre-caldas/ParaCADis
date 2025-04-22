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

#include <libparacadis/base/expected_behaviour/SharedPtr.h>
#include <libparacadis/base/threads/locks/reader_locks.h>
#include <libparacadis/base/threads/locks/writer_locks.h>

namespace Threads::SafeStructs
{
  /**
   * Encapsulates a SharedPtr<T> to use SharedLock and ExclusiveLock.
   *
   * Recommended: Just like an atomic SharedPtr.
   * Except that we use Threads::MutexData
   * so the mutex can be accessed as with other ThreadSafeXxxx in this library.
   * For example, you can have signals for when the pointer changes.
   * In this case, the pointer is protected, but the pointed class is not.
   *
   * @attention
   * We do not implement ReaderGate and WriterGate because the contents
   * pointed by the SharedPtr are not protected by the mutex.
   */
  template<typename T>
  class ThreadSafeSharedPtr
  {
  private:
    mutable Threads::MutexData mutex;
    SharedPtr<T>               theSharedPtr;

  public:
    using self_t   = ThreadSafeSharedPtr;

    ThreadSafeSharedPtr() = default;
    ThreadSafeSharedPtr(SharedPtr<T> shared_ptr);

    /**
     * Move constructor.
     * @attention We assume without check that no other threads have
     * access to the moved structure.
     */
    ThreadSafeSharedPtr(ThreadSafeSharedPtr&& other)
        : theSharedPtr(std::move(other.theSharedPtr)) {}

    // We could have a copy constructor.
    // But... do we want to?
//    ThreadSafeSharedPtr(const ThreadSafeSharedPtr& other);

    virtual ~ThreadSafeSharedPtr() = default;

    SharedPtr<T> getSharedPtr() const;
    SharedPtr<T> setSharedPtr(SharedPtr<T> shared_ptr);

  public:
    constexpr auto& getMutexLike() const { return mutex; }
  };

}  // namespace Threads::SafeStructs

#include "ThreadSafeSharedPtr.hpp"
