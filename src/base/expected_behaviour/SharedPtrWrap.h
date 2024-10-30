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

#include "SharedPtr.h"

#include <type_traits>

/**
 * A "transparent" SharedPtr.
 *
 * This is a SharedPtr that constructs its pointed object
 * and can be treated as a reference to it.
 */
template<typename T>
class SharedPtrWrap : private SharedPtr<T>
{
public:
  template<typename... Args>
  SharedPtrWrap(Args&&... args)
      : SharedPtr<T>(std::make_shared<T>(std::forward<Args>(args)...))
  {}

  operator const T&() const { return **this; }
  operator       T&()       { return **this; }

  template<typename Conv>
  operator Conv() const { return **this; }
  template<typename Conv>
  operator Conv()       { return **this; }

  const SharedPtr<T>& getSharedPtr() const { return *this; }

  using SharedPtr<T>::operator SharedPtr<const T>;
};
