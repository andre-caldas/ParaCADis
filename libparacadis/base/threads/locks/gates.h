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

#pragma once

#include "MutexData.h"

#include <base/expected_behaviour/SharedPtr.h>
#include <base/type_traits/deference_until.h>

#include <concepts>
#include <memory>

namespace Threads
{
  /**
   * Concept of a `GateInfo`.
   * A "gate" gives access to some protected data.
   * Usually, holding a "gate" implies having exclusive access
   * or at least having warranties that data access will be done
   * in a safe way, according to the designed access policy.
   *
   * @todo Check all the gates below.
   */
  template<typename T>
  concept C_GateInfo = requires(T a) {
    typename T::data_t;
    typename T::holder_t;

/*
    requires requires(T::holder_t& holder) {
      a.getData(holder);
//      {a.getData(holder)} -> std::same_as<T::data_t&>;
    };

    requires requires(const T::holder_t& const_holder) {
      a.getData(const_holder);
//      {a.getData(const_holder)} -> std::same_as<const T::data_t&>;

      a.getMutex(const_holder);
      {a.getMutexLike(const_holder)} -> C_MutexLike;
    };
*/
  };


  /**
   * Concept of a `MutexHolder` that implements access gates.
   * The `MutexHolderWithGates` must:
   * 1. Be a `MutexHolder`.
   * 2. Define a MutexHolder::GateInfo class.
   */
  template<typename T>
  concept C_MutexHolderWithGates = C_MutexHolder<T> && requires(T a) {
    typename T::GateInfo;
    requires C_GateInfo<typename T::GateInfo>;
//    requires std::same_as<T, typename T::GateInfo::holder_t>;
  };


  template<typename T>
  constexpr auto& getMutex(T& mutex)
  {
    if constexpr(C_MutexLike<T>) {
      return mutex;
    } else {
      auto& holder = deferenceUntilCheck<
          []<typename X> consteval {return C_MutexHolder<X>;}>(mutex);
      return holder.getMutexLike();
    }
  }


  /**
   * Information to implement gates exported by a C_MutexHolderWithGates.
   */
  /// @{
  /**
   * Just for template specialization.
   *
   * @attention
   * If you are getting the static_assert error, this means
   * you have to specialize DataTranslator and define the proper
   * translation between the structures you are using.
   * Or, you are just forgetting to include the propper `.h` file.
   * (or you are passing the wrong template parameters).
   */
  template<auto PTR_TO_MEMBER, auto PTR_TO_MUTEX>
  class LocalGateInfo
  {
    static_assert(sizeof(PTR_TO_MEMBER) == 0,
                  "Did you forget to include the specialization?");
  };


  /**
   * Info to export a member of a C_MutexHolder through a local pointer.
   *
   * Usage:
   * using GateInfo = LocalGateInfo<&SELF::exportedData>;
   */
  template<typename Holder,
           typename T, T Holder::* localData,
           typename M, M Holder::* localMutex>
  struct LocalGateInfo<localData, localMutex>
  {
    using data_t = T;
    using holder_t = Holder;

    static auto& getData(const Holder& holder)
    { return deferenceIfPossible(holder.*localData); }

    static auto& getData(Holder& holder)
    { return deferenceIfPossible(holder.*localData); }

    static auto& getMutex(const Holder& holder)
    { return Threads::getMutex(holder.*localMutex); }
  };
  /// @}


  /**
   * Information to implement gates exported by a C_MutexHolderWithGates
   * to other C_MutexHolderWithGates.
   */
  /// @{
  /// Just for template specialization.
  template<auto PTR_TO_MEMBER>
  class LocalBridgeInfo {};

  /**
   * Info to export a member of a C_MutexHolder through a local pointer.
   *
   * Usage:
   * using GateInfo = LocalGateInfo<&SELF::exportedData>;
   */
  template<typename Holder,
           typename T, T Holder::* localHolder>
  struct LocalBridgeInfo<localHolder>
  {
    using data_t = T;
    using holder_t = Holder;

    static auto& getData(const Holder& holder)
    {
      auto& other_holder = deferenceIfPossible(holder.*localHolder);
      using info_t = std::decay_t<decltype(other_holder)>::GateInfo;
      return info_t::getData(other_holder);
    }

    static auto& getData(Holder& holder)
    {
      auto& other_holder = deferenceIfPossible(holder.*localHolder);
      using info_t = std::decay_t<decltype(other_holder)>::GateInfo;
      return info_t::getData(other_holder);
    }

    static auto& getMutex(const Holder& holder)
    {
      auto& other_holder = deferenceIfPossible(holder.*localHolder);
      using info_t = std::decay_t<decltype(other_holder)>::GateInfo;
      return info_t::getMutex(other_holder);
    }
  };
  /// @}
}  // namespace Threads
