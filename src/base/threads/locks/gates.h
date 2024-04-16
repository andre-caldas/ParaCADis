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

#ifndef Threads_Gates_H
#define Threads_Gates_H

#include "MutexData.h"

#include <base/expected_behaviour/SharedPtr.h>
#include <base/type_traits/deference_until.h>

#include <concepts>
#include <memory>

namespace Threads
{

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
  /// Just for template specialization.
  template<auto PTR_TO_MEMBER, auto PTR_TO_MUTEX>
  class LocalGateInfo {};

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
    static auto& getData(const Holder& holder)
    { return deferenceIfPossible(holder.*localData); }

    static auto& getData(Holder& holder)
    { return deferenceIfPossible(holder.*localData); }

    static auto& getMutex(const Holder& holder)
    { return getMutex(holder.*localMutex); }
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

#endif
