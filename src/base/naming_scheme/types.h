// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2023-2024 André Caldas <andre.em.caldas@gmail.com>       *
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

#ifndef NamingScheme_Types_H
#define NamingScheme_Types_H

#include "PathToken.h"

#include <base/expected_behaviour/SharedPtr.h>

#include <concepts>
#include <ranges>
#include <vector>

template<typename T>
class WeakPtr;

namespace NamingScheme
{

  class ExporterCommon;

  using token_item = PathToken;
  template<typename R>
  concept C_TokenRange
      = std::ranges::range<R>
        && std::convertible_to<std::ranges::range_value_t<R>, const token_item&>;
  using token_vector   = std::vector<PathToken>;
  using token_iterator = std::ranges::subrange<token_vector::const_iterator>;
  static_assert(C_TokenRange<token_vector>);

}  // namespace NamingScheme

#endif
