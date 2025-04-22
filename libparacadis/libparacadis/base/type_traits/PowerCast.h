// SPDX-License-Identifier: LGPL-2.1-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2025 André Caldas <andre.em.caldas@gmail.com>            *
 *                                                                          *
 *   This file is part of FreeCAD.                                          *
 *                                                                          *
 *   FreeCAD is free software: you can redistribute it and/or modify it     *
 *   under the terms of the GNU Lesser General Public License as            *
 *   published by the Free Software Foundation, either version 2.1 of the   *
 *   License, or (at your option) any later version.                        *
 *                                                                          *
 *   FreeCAD is distributed in the hope that it will be useful, but         *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of             *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU       *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU Lesser General Public       *
 *   License along with FreeCAD. If not, see                                *
 *   <https://www.gnu.org/licenses/>.                                       *
 *                                                                          *
 ***************************************************************************/

#pragma once

#include <libparacadis/base/expected_behaviour/SharedPtr.h>

#include <type_traits>

namespace TypeTraits
{
  /**
   * Helper to execute the propper lambda function,
   * by dynamic_casting a base pointer to the correct
   * type among a list.
   *
   * This can simplify, for example, exporting templates to python.
   *
   * I am too lazy and I have asked chatgpt to generate it for me. :-)
   */
  template<typename Base, typename... Ts>
  struct PowerCast;

  template<typename Base, typename T, typename... Rest>
  struct PowerCast<Base, T, Rest...> {
    static_assert(std::is_base_of_v<Base, T>, "All types must derive from Base");

    template<typename Func>
    static bool dispatch(Base* ptr, Func&& func) {
      if(T* casted = dynamic_cast<T*>(ptr)) {
        func(casted);
        return true;
      }
      return PowerCast<Base, Rest...>::dispatch(ptr, std::forward<Func>(func));
    }

    template<typename Func>
    static bool dispatch(SharedPtr<Base> ptr, Func&& func) {
      if(auto casted = ptr.template cast<T>()) {
        func(std::move(casted));
        return true;
      }
      return PowerCast<Base, Rest...>::dispatch(std::move(ptr), std::forward<Func>(func));
    }
  };

  // Base case: no types left to try
  template<typename Base>
  struct PowerCast<Base> {
    template<typename Func>
    static bool dispatch(Base*, Func&&) {
      return false;
    }

    template<typename Func>
    static bool dispatch(SharedPtr<Base>, Func&&) {
      return false;
    }
  };
}
