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

#include "ThreadSafeStruct.h"

namespace Threads::SafeStructs
{
  template<typename Struct>
  ThreadSafeStruct<Struct>::ThreadSafeStruct(MutexLayer layer)
      : mutex(layer)
  {}

  template<typename Struct>
  ThreadSafeStruct<Struct>::ThreadSafeStruct(record_t&& record)
      : theStruct(std::move(record))
  {}

  template<typename Struct>
  ThreadSafeStruct<Struct>::ThreadSafeStruct(MutexLayer layer, record_t&& record)
      : mutex(layer)
      , theStruct(std::move(record))
  {}

  template<typename Struct>
  template<typename... T>
  ThreadSafeStruct<Struct>::ThreadSafeStruct(T&&... t)
      : theStruct(std::forward<T>(t)...) {}

  template<typename Struct>
  template<typename... T>
  ThreadSafeStruct<Struct>::ThreadSafeStruct(MutexLayer layer, T&&... t)
      : mutex(layer)
      , theStruct(std::forward<T>(t)...) {}
}
