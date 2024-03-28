// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2024 André Caldas <andre.em.caldas@gmail.com>            *
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

#ifndef Threads_WriterLocks_H
#define Threads_WriterLocks_H

#include "gates.h"
#include "LockPolicy.h"
#include "YesItIsAMutex.h"

#include <base/expected_behaviour/SharedPtr.h>

namespace Threads
{

  /**
   * Locks many mutexes exclsively (for writing).
   */
  template<C_MutexData FirstMutex, C_MutexData... Mutex>
  class ExclusiveLock : public LockPolicy
  {
  public:
    template<C_MutexHolder FirstHolder, C_MutexHolder... Holder>
    [[nodiscard]]
    ExclusiveLock(FirstHolder& first_holder, Holder&... holder);

    void release();

    [[maybe_unused]]
    auto detachFromThread();

  private:
    using locks_t = std::scoped_lock<
        YesItIsAMutex, TypeTraits::ForEach_t<YesItIsAMutex, Mutex>...>;
    /*
     * After constructed, std::scoped_lock cannot be changed.
     * So, we usa a smart pointer.
     * We could, of course, simply not use a scoped_lock,
     * and lock on construction and unlock on destruction.
     * But, unfortunately, std::lock needs two or more mutexes,
     * and we do not want the code full of ifs.
     *
     * We use a shared_ptr because when a new thread is created,
     * we want them both to hold the lock, while we store the new thread
     * information: the std::thread instance.
     */
    std::shared_ptr<locks_t> locks;
  };

  template<C_MutexHolder FirstHolder, C_MutexHolder... Holder>
  ExclusiveLock(FirstHolder&, Holder&...)
      -> ExclusiveLock<MutexData, TypeTraits::ForEach_t<MutexData, Holder>...>;

  /**
   * Writer gate.
   *
   * @attention If you need more than one mutex, you have to allocate
   * all of them at once using ExclusiveLock. Only then,
   * you instantiate the WriterGate.
   */
  /// @{
  /// Allow template specialization.
  template<auto PTR_TO_MEMBER>
  class WriterGate {};

  template<C_MutexHolder Holder, typename T, T Holder::* localData>
  class WriterGate<localData> : public _GateBase<ExclusiveLock<MutexData>>
  {
  public:
    WriterGate(Holder& holder);

    T& operator*() { return *data; }
    T* operator->() { return data; }

  private:
    T* data;
  };

}  // namespace Threads

#include "writer_locks_impl.h"

#endif
