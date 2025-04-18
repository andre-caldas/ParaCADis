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

#include "Uuid.h"

#include "exceptions.h"

#include <base/xml/streams.h>

#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <regex>

namespace std
{
  size_t hash<boost::uuids::uuid>::operator()(const boost::uuids::uuid& uuid) const noexcept
  {
    return boost::uuids::hash_value(uuid);
  }
}

namespace
{
  boost::uuids::random_generator random_generator;
  boost::uuids::string_generator string_generator;
  boost::uuids::nil_generator    zero_generator;
}

using namespace Naming;

Uuid::Uuid() : uuid(random_generator())
{
}

Uuid::Uuid(int i) : uuid(zero_generator())
{
  assert(i == 0);
}

Uuid::Uuid(std::string_view uuid_str)
try : uuid(string_generator(uuid_str.cbegin(), uuid_str.cend())) {
} catch (const std::runtime_error&) {
  // TODO: name an exception.
  throw;
}

bool Uuid::isValid() const
{
  return !uuid.is_nil();
}

bool Uuid::isValid(std::string_view uuid_str)
{
  // Unfortunately, boost does not implement a method
  // to check for valid uuid strings.
  static const std::regex pattern(
      "[{]?"
      "[0-9a-fA-F]{8}-?"
      "[0-9a-fA-F]{4}-?"
      "[0-9a-fA-F]{4}-?"
      "[0-9a-fA-F]{4}-?"
      "[0-9a-fA-F]{12}-?"
      "[}]?",
      std::regex::optimize
  );
  return std::regex_match(uuid_str.cbegin(), uuid_str.cend(), pattern);
}


void Uuid::serialize(Xml::Writer& /*writer*/) const noexcept
{
  assert(false);
}

Uuid Uuid::unserialize(Xml::Reader& /*reader*/)
{
  throw Exception::NotImplemented{};
}
