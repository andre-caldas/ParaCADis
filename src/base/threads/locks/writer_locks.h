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
  template<C_MutexGatherOrData... Mutex>
  class ExclusiveLock : public LockPolicy
  {
  public:
    [[nodiscard]]
    ExclusiveLock(Mutex&... mutex);

    void release();

    [[maybe_unused]]
    auto detachFromThread();

  private:
    using locks_t = decltype(
        lockThemAll<Mutex...>(*(Mutex*)nullptr...))::element_type;

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


  template<C_MutexHolderWithGates ... Holders>
  class WriterGate
      : ExclusiveLock<typename Holders::mutex_data_t...>
  {
  public:
    WriterGate(Holders&... holders);

    template<C_MutexHolderWithGates Holder>
    auto& operator[](Holder& holder) const;

  private:
#ifndef NDEBUG
    const std::unordered_set<const void*> all_holders;
#endif
  };


  template<C_MutexHolderWithGates Holder>
  class WriterGate<Holder>
      : ExclusiveLock<typename Holder::mutex_data_t>
  {
  public:
    WriterGate(Holder& holder);

    auto& operator*() { return data; }
    auto* operator->() { return &data; }

  private:
    Holder::WriterGate::protected_data_t& data;
  };


  /**
   * A local writer gate.
   *
   * @attention If you need more than one mutex, you have to allocate
   * all of them at once using ExclusiveLock. Only then,
   * you instantiate the WriterGate.
   */
  /// @{
  /// Allow template specialization.
  template<auto PTR_TO_MEMBER>
  class LocalWriterGate {};

  template<C_MutexHolder Holder, typename T, T Holder::* localData>
  class LocalWriterGate<localData> : public _GateBase<ExclusiveLock<MutexData>>
  {
  public:
    LocalWriterGate(Holder& holder);

    using protected_data_t = T;

    T& operator*() { return data; }
    T* operator->() { return &data; }

  private:
    T& data;

    static T& getProtectedData(Holder& holder) { return holder.*localData; }

    template<C_MutexHolderWithGates... Holders>
    friend class WriterGate;
  };
  /// @}

}  // namespace Threads

#include "writer_locks_impl.h"

#endif
