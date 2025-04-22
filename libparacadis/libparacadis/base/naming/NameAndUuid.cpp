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

#include "NameAndUuid.h"

#include "PathToken.h"
#include "exceptions.h"

#include <libparacadis/base/xml/streams.h>

#include <cassert>

using namespace Naming;

NameAndUuid::NameAndUuid(const Uuid& _uuid) : uuid(_uuid)
{
}

NameAndUuid::NameAndUuid(const Uuid& _uuid, std::string _name)
    : uuid(_uuid)
    , name(std::move(_name))
{
}

bool NameAndUuid::isValidName(std::string_view name_str)
{
  if (name_str.empty()) { return true; }
  return !Uuid::isValid(name_str);
}

void NameAndUuid::setName(std::string name_str)
{
  assert(uuid.isValid());  // We have a valid uuid.
  if (!isValidName(name_str)) { throw Exception::InvalidName(std::move(name_str)); }
  name = std::move(name_str);
}

bool NameAndUuid::pointsToMe(const PathToken& name_or_uuid) const
{
  if (name_or_uuid.isUuid()) { return (uuid == name_or_uuid.getUuid()); }
  return !name.empty() && (name == name_or_uuid.getName());
}

std::string NameAndUuid::toString() const
{
  if (hasName()) { return name; }
  return uuid;
}

void NameAndUuid::serialize(Xml::Writer& /*writer*/) const noexcept
{
  assert(false);
}

NameAndUuid NameAndUuid::unserialize(Xml::Reader& /*reader*/)
{
  throw Exception::NotImplemented{};
}
