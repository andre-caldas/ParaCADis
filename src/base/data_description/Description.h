// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2025 André Caldas <andre.em.caldas@gmail.com>            *
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

#include <base/type_traits/NamedMember.h>

#include <concepts>
#include <tuple>

namespace DataDescription
{
  class DescriptionBase
  {
  public:
    virtual const char* describeClass() const;
    virtual const char* describe(void* ptr) const;
  };

  template<typename T>
  concept C_Description = std::derived_from<T, DescriptionBase>;

  /**
   * Template to be specialized.
   *
   * @attention
   * If you are getting the static_assert error, this means
   * you have to specialize DataTranslator and define the proper
   * translation between the structures you are using.
   * Or, you are just forgetting to include the propper `.h` file.
   *
   * @example
   * template<>
   * class Description<Struct>
   * : public DescriptionT<Struct, "Struct Name",
   *                       {&Struct::radius, "Radius"},
   *                       {&Struct::center, "Center"},
   *                       {&Struct::normal, "Normal"}>
   * {};
   */
  template<typename T>
  class Description
  {
    static_assert(sizeof(T) == 0,
                  "Did you forget to include the specialization?");
  };


  template<typename Struct, TemplateString struct_name,
           TypeTraits::NamedMember... named_items>
  class DescriptionT
    : public DescriptionBase
  {
  public:
    Struct& data;

    // Behave as if it was a Struct.
    DescriptionT(Struct& _data) : data(_data) {}
    operator Struct&() {return data;}

    const char* describeClass() const override;
    const char* describe(void* ptr) const override;

    std::tuple<> items{named_items...};
  };
}

#include "Description.hpp"
