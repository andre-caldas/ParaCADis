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

#include "PathToken.h"

#include "exceptions.h"

#include <libparacadis/base/xml/streams.h>

#include <cassert>

using namespace Naming;

PathToken::PathToken(std::string name_or_uuid)
{
  assert(!name_or_uuid.empty());
  if(Uuid::isValid(name_or_uuid)) {
    uuid = Uuid{std::move(name_or_uuid)};
  } else {
    uuid = Uuid{0};
    name = std::move(name_or_uuid);
  }
}

std::string PathToken::toString() const
{
  if (isName()) { return name; }
  return uuid;
}

void PathToken::serialize(Xml::Writer& /*writer*/) const noexcept
{
  assert(false);
}

PathToken PathToken::unserialize(Xml::Reader& /*reader*/)
{
  throw Exception::NotImplemented{};
}
