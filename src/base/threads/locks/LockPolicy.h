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

#include "../type_traits/Utils.h"
#include "YesItIsAMutex.h"

#include <concepts>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <type_traits>
#include <unordered_set>

namespace Threads
{

  class MutexData;

  /**
   * Concept of a `MutexHolder`.
   * The `MutexHolder` must define a `getMutexData()` method.
   */
  template<typename T>
  concept C_MutexHolder = requires (T a) {
    { a.getMutexData() } -> std::convertible_to<MutexData*>;
  }

  /**
   * Concept of a `MutexHolder` that implements access gates.
   * The `MutexHolderWithGates` must:
   * 1. Be a `MutexHolder`.
   * 2. Define a MutexHolder::WriterGate class
   *    that implements the container methods that demand ExclusiveLock.
   * 3. Define a method that takes an ExclusiveLock as argument,
   *    and returns a WriterGate instance.
   */
  template<typename T>
  concept C_MutexHolderWithGates = C_MutexHolder<T> && requires (T a) {
    using R = typename T::ReaderGate;
    { a.getReaderGate((SharedLock*)nullptr) } -> std::convertible_to<const R&>;
    using W = typename T::WriterGate;
    { a.getWriterGate((ExclusiveLock*)nullptr) } -> std::convertible_to<const W&>;
  }

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
    template<IsMutxexPair... MutN>
    LockPolicy(bool is_exclusive, MutN*... mutex);

    LockPolicy() = delete;
    virtual ~LockPolicy();

    bool isDetached() const;
    bool hasIgnoredMutexes() const;

    int minLayerNumber() const;
    int maxLayerNumber() const;

    const std::unordered_set<const MutexData*>& getMutexes() const;

  private:
    bool is_detached         = true;
    bool has_ignored_mutexes = false;

    std::unordered_set<const MutexData*> mutexes;

    void _processLock(bool is_exclusive);
    void _processExclusiveLock();
    void _processNonExclusiveLock();
    /**
     * @brief Removes information from thread_local variables.
     */
    void _detachFromThread();
  };


  class SharedLock : public LockPolicy
  {
  public:
    [[nodiscard]] SharedLock(MutexData& mutex);

  private:
    std::shared_lock<YesItIsAMutex> lock;
  };


  /**
   * Locks many mutexes exclsively (for writing).
   */
  template<C_MutexData FirstMutex, C_MutexData... MutexData>
  class ExclusiveLock : public LockPolicy
  {
  public:
    [[nodiscard]]
    ExclusiveLock(FirstMutex& first_mutex, MutexData&... mutex_data);

    void release();

    [[maybe_unused]] auto detachFromThread();

  private:
    using locks_t
        = std::scoped_lock<YesItIsAMutex, ForEach_t<YesItIsAMutex, MutexData>...>;
    /*
     * After constructed, std::scoped_lock cannot be changed.
     * So, we usa a unique_ptr.
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


  /**
   * @brief Locks and gives access to locked classes of type "MutexHolder".
   */
  template<C_MutexHolder FirstHolder, C_MutexHolder... MutexHolder>
  class ExclusiveLockGate : public ExclusiveLock
  {
  public:
    [[nodiscard]]
    ExclusiveLockGate(FirstHolder& first_holder, MutexHolder&... mutex_holder);

    // This could actually be static.
    template<C_MutexHolderWithGate SomeHolder>
    auto& operator[](SomeHolder& tsc) const;

    template<typename = std::enable_if_t<sizeof...(MutexHolder) == 0>>
    auto operator->() const;

  private:
    FirstHolder& FirstHolder;
  };

}  // namespace Threads

#include "LockPolicy.inl"

#endif  // Threads_LockPolicy_H

