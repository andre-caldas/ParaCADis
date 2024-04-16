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

#include <base/type_traits/Utils.h>

#include <cassert>
#include <concepts>
#include <limits>
#include <memory>
#include <mutex>
#include <type_traits>
#include <unordered_set>
#include <vector>

namespace Threads
{

  /**
   * The lock policy uses a MutexData to represent one mutex.
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
    MutexData(const MutexData&) = delete;
    MutexData& operator=(const MutexData&) = delete;
  };

  template<typename T>
  concept C_MutexData = std::same_as<std::remove_cvref_t<T>, MutexData>;


  struct MutexGatherBase {};

  template<typename... T>
  struct GatherMutexData : MutexGatherBase {};

  template<typename First, typename... MutexDataFormat>
  struct GatherMutexData<First, MutexDataFormat...> : MutexGatherBase
  {
    constexpr GatherMutexData(First& first, MutexDataFormat&... others)
        : first(first), others(others...) {}
    First& first;
    GatherMutexData<MutexDataFormat...> others;
  };

  template<typename T>
  concept C_MutexGather = std::derived_from<std::remove_cvref_t<T>, MutexGatherBase>;

  template<typename T>
  concept C_EmptyMutexGather = std::same_as<std::remove_cvref_t<T>, GatherMutexData<>>;

  using DummyMutex = GatherMutexData<>;

  /**
   * Indicates if the type can be converted at compile time to a pack of MutexData.
   */
  template<typename T>
  concept C_MutexGatherOrData = C_MutexGather<T> || C_MutexData<T>;


  constexpr std::vector<MutexData*>
  getPlainMutexes() { return {}; }

  template<C_MutexGatherOrData First, C_MutexGatherOrData... MutexDataFormat>
  constexpr std::vector<MutexData*>
  getPlainMutexes(First& f, MutexDataFormat&... m)
  {
    if constexpr(C_MutexData<First> && (C_MutexData<MutexDataFormat> && ...)) {
      return {&f, &m...};
    } else if constexpr (C_MutexData<First>) {
      return getPlainMutexes(m..., f);
    } else if constexpr (C_EmptyMutexGather<First>) {
      return getPlainMutexes(m...);
    } else {
      return getPlainMutexes(f.first, f.others, m...);
    }
  }

  class MutexVector
  {
  private:
    using set_t = std::vector<MutexData*>;
    set_t mutexes;

  public:
    template<C_MutexGatherOrData... MutexLike>
    constexpr MutexVector(MutexLike&... m)
        : mutexes(getPlainMutexes(m...)) {}

    template<typename... MutexLike>
    constexpr MutexVector(MutexLike&... m)
        : mutexes(unfold({}, m...)) {}

    /// @attention Might repeat mutexes.
    auto cbegin() const { return mutexes.cbegin(); }
    auto cend() const { return mutexes.cend(); }

    operator std::unordered_set<MutexData*>()
    { return {mutexes.cbegin(), mutexes.cend()}; }

  private:
    set_t unfold(set_t&& set) { return set; }

    template<typename First, typename... MutexLike>
    set_t unfold(set_t&& set, First& f, MutexLike&... m)
    {
      if constexpr((C_MutexData<First> && ... && C_MutexData<MutexLike>)) {
        if(set.empty()) {
          return {&f, &m...};
        }
        set_t other_set = {&f, &m...};
        set.insert(set.end(), other_set.cbegin(), other_set.cend());
        return std::move(set);
      } else if constexpr(C_MutexData<First>) {
        // Just rotate so we can add all of them at the end, at once.
        return unfold(std::move(set), m..., f);
      } else if constexpr (C_EmptyMutexGather<First>) {
        // Removes the empty gathering.
        return unfold(std::move(set), m...);
      } else if constexpr(C_MutexGather<First>) {
        // Splits the gathering in two.
        return unfold(std::move(set), f.first, f.other, m...);
      } else if constexpr(std::same_as<std::remove_cvref_t<First>, MutexVector>) {
        // Consumes the MutexVector by aggregating it to the "set".
        set.insert(set.end(), f.cbegin(), f.cend());
        return unfold(std::move(set), m...);
      } else {
        static_assert(false,
                      "We can only unfold MutexData, MutexGather or MutexVector.");
      }
    }
  };

  template<typename T>
  concept C_MutexVector = std::same_as<std::remove_cvref_t<T>, MutexVector>;

  template<typename T>
  concept C_MutexLike = C_MutexGatherOrData<T> || C_MutexVector<T>;


  /**
   * Concept of a `MutexHolder`.
   * The `MutexHolder` be convertible to its mutex.
   */
  template<typename T>
  concept C_MutexHolder = requires(T a) {
    a.getMutexLike();
    {a.getMutexLike()} -> C_MutexLike;
  };

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
  concept C_MutexHolderWithGates = C_MutexHolder<T> && requires(T a) {
    // Type of the struct that holds the data for each record.
//    typename T::record_t;

    typename T::GateInfo;
//    T::GateInfo::getData;
//    T::GateInfo::getMutex;
//    std::invoke<T::GateInfo::getData, ???>;
//    std::invoke<T::GateInfo::getMutex, ???>;
  };

}  // namespace Threads

#endif
