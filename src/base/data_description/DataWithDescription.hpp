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

#include "DataWithDescription.h"

namespace DataDescription
{
  template<typename Struct, TemplateString struct_name,
           TypeTraits::NamedMember... named_items>
  const char*
  DataWithDescriptionT<Struct, struct_name, named_items...>::describe_class() const
  {
    return struct_name;
  }

  template<typename Struct, TemplateString struct_name,
           TypeTraits::NamedMember... named_items>
  const char*
  DataWithDescriptionT<Struct, struct_name, named_items...>::describe(void* ptr) const
  {
    for(const auto& item: {named_items...}) {
      if(ptr == &(this->*(item.local_ptr))) {
        return this->*(item.name);
      }
    }
    return DataWithDescriptionBase::describe(ptr);
  }
}

#include "DataWithDescription.hpp"
