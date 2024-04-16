// SPDX-License-Identifier: LGPL-2.1-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2023 André Caldas <andre.em.caldas@gmail.com>            *
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

#ifndef BASE_Threads_deference_until_H
#define BASE_Threads_deference_until_H

#include <concepts>

template<typename T>
concept C_Deferenceable = requires(T a) {
  *a;
};

template<typename T>
constexpr auto& deferenceIfPossible(T& a)
{
  if constexpr(C_Deferenceable<T>) { return *a; }
  else { return a; }
}

/**
 * Unfortunately, we cannot pass concepts as template parameters.
 *
 * Usage:
 * deferenceUntilCheck<[]<typename T> consteval {return Concept<T>;}>(x);
 */
template<auto Check, typename T>
constexpr auto& deferenceUntilCheck(T& a)
{
  if constexpr(!Check.template operator()<T>()) {
    if constexpr(C_Deferenceable<T>) {
      return deferenceUntilConcept<Check>(*a);
    } else {
      static_assert(C_Deferenceable<T>,
                    "Type cannot be deferenced until reaches required concept.");
    }
  }
  static_assert(Check.template operator()<T>(),
                "Implementation bug. This should never happen.");
  return a;
}

#endif
