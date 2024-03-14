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

#ifndef BASE_Accessor_Types_H
#define BASE_Accessor_Types_H

#include <base/expected_behaviour/SharedPtr.h>

#include <ranges>
#include <vector>

template<typename T>
class WeakPtr;

class Exporter;

namespace NamingScheme
{

  class NameOrUuid;
  class Exporter;

  using token_item         = NameOrUuid;
  using token_range        = std::ranges<token_item>;
  using token_iterator     = std::ranges::subrange<token_item>;
  using weak_ptr_collector = std::vector<WeakPtr<Exporter>>;

}  // namespace NamingScheme

#endif  // BASE_Accessor_Types_H

