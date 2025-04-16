// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2024 André Caldas <andre.em.caldas@gmail.com>            *
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

#include "exceptions.h"

#include <format>

namespace NamingScheme::Exception
{
  InvalidName::InvalidName(std::string_view name, std::source_location _location)
      : RunTimeError(std::format("Name cannot look like a UUID ({}).", name), _location)
  {
  }

  CannotResolve::CannotResolve(
      SharedPtr<ExporterBase> /*parent_lock*/,
      const token_iterator& /*tokens*/, std::source_location _location)
      : RunTimeError("Cannot resolve accessor reference.", _location)
  {
  }

  NoExport::NoExport(std::source_location _location)
      : RunTimeError("Object does not export required type.", _location)
  {
  }
}
