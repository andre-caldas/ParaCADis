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

namespace DataDescription
{
  class DataWithDescriptionBase
  {
  public:
    virtual const char* describe_class() const;
    virtual const char* describe(void* ptr) const;
  };

  template<typename T>
  concept C_DataWithDescription = std::derived_from<T, DataWithDescriptionBase>;

  /**
   * Template to be specialized.
   *
   * @attention
   * If you are getting linker errors,
   * this means you have to specialize DataWithDescription
   * for the structure you are using.
   */
  template<typename T>
  class DataWithDescription;

  template<typename Struct, TemplateString struct_name,
           TypeTraits::NamedMember... named_items>
  class DataWithDescriptionT
    : public DataWithDescriptionBase
  {
  public:
    Struct data;

    DataWithDescriptionT() = default;

    // Behave as if it was a Struct.
    DataWithDescriptionT(const Struct& data) : data(data) {}
    DataWithDescriptionT(Struct&& data) : data(std::move(data)) {}
    operator Struct&() {return data;}

    const char* describe_class() const override;
    const char* describe(void* ptr) const override;
  };
}

#include "DataWithDescription.hpp"
