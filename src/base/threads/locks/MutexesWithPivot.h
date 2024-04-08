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

#ifndef Threads_MutexesWithPivot_H
#define Threads_MutexesWithPivot_H

#include "MutexData.h"

#include <concepts>

namespace Threads
{

  struct MutexesWithPivotBase {};

  template<typename... M>
  struct MutexesWithPivot
      : MutexesWithPivotBase
      , GatherMutexData<MutexData, M...>
  {
    MutexData pivot;
    MutexesWithPivot(M&... m)
        : GatherMutexData<MutexData, M...>{pivot, m...}
    {
      for(auto* mut: {&m...}) {
        mut->setPivot(pivot);
      }
    }
  };


  /**
   * Concept of a `MutexesWithPivot`.
   */
  template<typename T>
  concept C_MutexesWithPivot = std::derived_from<T, MutexesWithPivotBase>;


  template<typename First, typename... M>
  constexpr auto
  lockAllExclusive(First& f, M&... m)
  {
    if constexpr ((C_MutexData<First>) && (... && C_MutexData<M>)) {
      // f.pivot and m.pivot... will be treated by LockPolicy.
      return std::make_unique<
          std::scoped_lock<YesItIsAMutex, TypeTraits::ForEach_t<YesItIsAMutex, M>...>
      >(f.mutex, m.mutex...);
    }
    else if constexpr (C_MutexData<First>) {
      // Is the first a MutexData?
      return lockAllExclusive(m..., f);
    }
    else if constexpr (std::same_as<const First, const GatherMutexData<>>) {
      // Remove f if it is an empty list?
      return lockAllExclusive(m...);
    }
    else if constexpr (C_MutexesWithPivot<First>)
    {
      // We only lock the pivot.
      return lockAllExclusive(f.pivot, m...);
    }
    else
    {
      // Split First into pices.
      return lockAllExclusive(f.first, f.others, m...);
    }
  }

  constexpr auto
  lockAllExclusive()
  {
    return std::make_unique<std::scoped_lock<>>();
  }

}  // namespace Threads

#endif
