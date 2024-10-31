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

#include <concepts>
#include <type_traits>

template<class T>
concept C_HasInit = requires (T& a) {
  a.init();
};

/**
 * A "transparent" SharedPtr.
 *
 * This is a SharedPtr that constructs its pointed object
 * and can be treated as a reference to it.
 *
 * It also replaces std::make_shared. You can use the macro ONLY_SHAREDPTRWRAP()
 * to make it a friend of T and then declare T's constructors private.
 * A good thing is that this "make_shared" also calls an init() method
 * if T has one. This is useful because when init() is called
 * the object is totally constructed. For instance,
 * getSelfSharedPtr() can be called.
 */
template<typename T>
class SharedPtrWrap
    : public SharedPtr<T>
{
private:
//  SharedPtr<T> ptr;

public:
  template<typename... Args>
  SharedPtrWrap(Args&&... args)
      : SharedPtr<T>(std::shared_ptr<T>(new T(std::forward<Args>(args)...)))
  {
    if constexpr(C_HasInit<T>) {
      (*this)->init();
    }
  }

  operator const T&() const { return **this; }
  operator       T&()       { return **this; }

  template<typename Conv> requires std::convertible_to<T, Conv>
  operator Conv() const { return **this; }
  template<typename Conv> requires std::convertible_to<T, Conv>
  operator Conv()       { return **this; }

  template<typename Conv> requires (!std::convertible_to<T, Conv>)
  operator Conv() const { return *this; }
  template<typename Conv> requires (!std::convertible_to<T, Conv>)
  operator Conv()       { return *this; }

  const SharedPtr<T>& getSharedPtr() const { return *this; }
  operator SharedPtr<T>() && { return std::move(*this); }
  operator const SharedPtr<T>&() const& { return *this; }
  operator SharedPtr<const T>() const { return *this; }
};

#define ONLY_SHAREDPTRWRAP(...)                    \
  template<typename _ONLY_SHAREDPTRTYPE>           \
  friend class SharedPtrWrap;
