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

#include <limits>
#include <memory>
#include <mutex>
#include <type_traits>

namespace Threads
{

  /**
   * The lock policy uses a @class MutexData to represent one mutex.
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
  concept C_MutexData = std::same_as<const T, const MutexData>;


  struct GatherMutexDataBase {};

  template<typename... M>
  struct GatherMutexData : GatherMutexDataBase {
    static_assert(sizeof...(M) == 0, "GatherMutexData must be specialized.");
    static constexpr std::size_t size() { return 0; }
  };

  template<typename First, typename... M>
  struct GatherMutexData<First, M...> : GatherMutexDataBase
  {
    First& first;
    GatherMutexData<M...> others;

    constexpr GatherMutexData(First& f, M&... m) : first(f), others{m...} {}
    static constexpr std::size_t size()
    {
      if constexpr(!C_MutexData<First>){
        return First::size() + decltype(others)::size();
      } else {
        return 1 + decltype(others)::size();
      }
    }
  };

  template<typename T>
  concept C_GatherMutexData = std::derived_from<T, GatherMutexDataBase>;

  template<typename T>
  concept C_MutexGatherOrData = C_MutexData<T> || C_GatherMutexData<T>;

  template<typename First, typename... M>
  constexpr auto
  lockThemAll(First& f, M&... m)
  {
    if constexpr ((C_MutexData<First>) && (... && C_MutexData<M>)) {
      return std::make_unique<
          std::scoped_lock<YesItIsAMutex, TypeTraits::ForEach_t<YesItIsAMutex, M>...>
      >(f.mutex, m.mutex...);
    }
    else
    {
      // Is the first a MutexData?
      if constexpr (C_MutexData<First>) {
        return lockThemAll(m..., f);
      }
      else
      {
        // Remove f.first if it is an empty list?
        if constexpr (std::same_as<decltype(f.first), GatherMutexData<>>) {
          return lockThemAll(f.others, m...);
        }
        else
        {
          // Split First into pices.
          return lockThemAll(f.first, f.others, m...);
        }
      }
    }
  }

  constexpr auto
  lockThemAll()
  {
    return std::make_unique<std::scoped_lock<>>();
  }


  /**
   * Concept of a `MutexHolder`.
   * The `MutexHolder` be convertible to its mutex.
   */
  template<typename T>
  concept C_MutexHolder = requires(T a) {
    { a } -> std::convertible_to<MutexData&>;
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

    typename T::ReaderGate;
//    a.getReaderGate();
//    typename T::WriterGate;
//    a.getWriterGate();
  };

}  // namespace Threads

#endif
