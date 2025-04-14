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

#include <base/expected_behaviour/SharedPtrWrap.h>

/// @todo Move somewhere else.
template<std::size_t N>
struct TemplateString
{
  constexpr TemplateString(const char (&str)[N])
  {
    // TODO: temporary check. Remove in future.
    static_assert(N <= 40, "Exported variable's name is too long.");
    std::ranges::copy(str, string);
  }

  constexpr operator const char*() const {return string;}

  char string[N];
};

namespace TypeTraits
{
  template<typename T, class C, std::size_t N>
  struct NamedMember
  {
    constexpr NamedMember(T C::* local_ptr, const char (&str)[N])
        : local_ptr(local_ptr)
        , name(str)
    {}

    constexpr NamedMember(T C::* local_ptr, TemplateString<N> str)
        : local_ptr(local_ptr)
        , name(str)
    {}

    using data_type = T;
    using pointer_to_type = T*;
    T C::* local_ptr;
    TemplateString<N> name;
  };

  template<typename T, class C, std::size_t N>
  struct NamedMember<SharedPtrWrap<T>, C, N>
  {
    constexpr NamedMember(SharedPtrWrap<T> C::* local_ptr, const char (&str)[N])
        : local_ptr(local_ptr)
        , name(str)
    {}

    constexpr NamedMember(SharedPtrWrap<T> C::* local_ptr, TemplateString<N> str)
        : local_ptr(local_ptr)
        , name(str)
    {}

    using data_type = T;
    using pointer_to_type = SharedPtrWrap<T>;
    SharedPtrWrap<T> C::* local_ptr;
    TemplateString<N> name;
  };

  template<typename T, typename P, typename NamedData>
  concept C_NamedDataOfType =
      (std::same_as<T, typename NamedData::data_type>)
      &&
      (std::same_as<P, typename NamedData::pointer_to_type>);

  template<typename T, typename P, typename... NamedData>
  concept C_AllNamedDataOfType = (C_NamedDataOfType<T, P, NamedData> &&...);
}
