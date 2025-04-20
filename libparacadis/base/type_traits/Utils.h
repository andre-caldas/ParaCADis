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

#include "IndexTraits.h"

#include <type_traits>

namespace TypeTraits
{
  template<typename T, template<typename> class Template>
  struct IsSpecializationOf : std::false_type {};

  template<typename U, template<typename> class Template>
  struct IsSpecializationOf<Template<U>, Template> : std::true_type {};

  template<typename T, template<typename> class Template>
  concept C_SpecializationOf = IsSpecializationOf<T, Template>::value;

  /**
   * @brief Used to unpack packed template types.
   * @see ForEach_t.
   */
  template<typename Result, typename From>
  struct ForEach {
    using type = Result;
  };
  /**
   * @brief When you have template packed types Fn,
   * you can use @class ForEach to replace each Fn by some
   * constant type Result:
   * ForEach_t<std::string, Fn>...
   */
  template<typename Result, typename From>
  using ForEach_t = typename ForEach<Result, From>::type;


  namespace utils_detail
  {
    // https://stackoverflow.com/a/72263473/1290711
    template<typename T>
    struct MemberPointerToAux;

    template<class C, typename T>
    struct MemberPointerToAux<T C::*> {
      using type = T;
    };
  }  // namespace utils_detail

  /**
   * @brief Given a "member pointer" T that points to a member of X,
   * informs which type is X.
   */
  template<auto MP>
  struct MemberPointerTo
      : utils_detail::MemberPointerToAux<std::remove_reference_t<decltype(MP)>> {
  };

  /**
   * @brief Given a "member pointer" T that points to a member of X,
   * the type X.
   */
  template<auto MP>
  using MemberPointerTo_t = typename MemberPointerTo<MP>::type;
}

namespace strip_detail
{
  template<typename T>
  struct StripSmartPointerAux {
    StripSmartPointerAux(T& _t) : t(_t) {}
    T& operator()() { return t; }
    T& t;
  };

  template<typename T>
  struct StripSmartPointerAux<std::shared_ptr<T>> : StripSmartPointerAux<T> {
    StripSmartPointerAux(std::shared_ptr<T>& t)
        : StripSmartPointerAux<T>(*t.get())
    {
    }
  };

  template<typename T>
  struct StripSmartPointerAux<const std::shared_ptr<T>>
      : StripSmartPointerAux<T> {
    StripSmartPointerAux(const std::shared_ptr<T>& t)
        : StripSmartPointerAux<T>(*t.get())
    {
    }
  };
}  // namespace strip_detail

template<typename T>
struct StripSmartPointer : strip_detail::StripSmartPointerAux<T> {
  using parent_t = strip_detail::StripSmartPointerAux<T>;
  StripSmartPointer(T& t) : parent_t(t) {}
};
