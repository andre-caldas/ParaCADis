// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2023 André Caldas <andre.em.caldas@gmail.com>            *
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

#ifndef NamedScheme_Exception_H
#define NamedScheme_Exception_H

#include <base/exceptions.h>
#include <base/expected_behaviour/shared_ptr.h>

#include <string>

namespace NamedScheme
{

  class ExceptionInvalidName : public Base::RunTimeError
  {
  public:
    ExceptionInvalidName(
        std::string_view     name,
        std::source_location location = std::source_location::current());
  };

  class ExceptionCannotResolve : public Base::RunTimeError
  {
  public:
    ExceptionCannotResolve(
        const shared_ptr<Exporter>& parent_lock,
        std::ranges::subrange<token_range>      tokens,
        std::source_location location = std::source_location::current());
  };

  class ExceptionNoExport : public Base::RunTimeError
  {
  public:
    ExceptionNoExport(
        std::source_location location = std::source_location::current());
  };

}  // namespace NamedScheme

#endif  // NamedScheme_Exception_H

