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

#include "exceptions.h"

#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace std
{
  size_t hash<boost::uuids::uuid>::operator()(const boost::uuids::uuid& uuid) const noexcept
  {
    return boost::uuids::hash_value(uuid);
  }
}

namespace NamingScheme
{

  /*
   * Uuid
   * ====
   */

  namespace
  {
    boost::uuids::random_generator random_generator;
    boost::uuids::string_generator string_generator;
    boost::uuids::nil_generator    zero_generator;
  }

  Uuid::Uuid()
      : uuid(random_generator())
  {
  }

  Uuid::Uuid(int i)
      : uuid(zero_generator())
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


  /*
   * NameOrUuid
   * ==========
   */

  NameOrUuid::NameOrUuid(std::string name_or_uuid)
      : uuid(name_or_uuid)
  {
    assert(!name_or_uuid.empty());
    if (!uuid.isValid()) {
      name = std::move(name_or_uuid);
    }
  }

  std::string NameOrUuid::toString() const
  {
    if (isName()) { return name; }
    return uuid;
  }


  /*
   * NameAnduuid
   * ===========
   */

  NameAndUuid::NameAndUuid(const Uuid& uuid)
      : Uuid(uuid)
  {
  }

  NameAndUuid::NameAndUuid(const Uuid& uuid, std::string name)
      : Uuid(uuid)
      , name(std::move(name))
  {
  }

  bool NameAndUuid::isValidName(std::string_view name_str)
  {
    if (name_str.empty()) {
      return true;
    }
    Uuid test(name_str);
    return !test.isValid();
  }

  void NameAndUuid::setName(std::string name_str)
  {
    assert(isValid());  // We have a valid uuid.
    if (!isValidName(name_str)) {
      throw Exception::InvalidName(std::move(name_str));
    }
    name = std::move(name_str);
  }

  bool NameAndUuid::pointsToMe(const NameOrUuid& name_or_uuid) const
  {
    if (name_or_uuid.isUuid()) {
      return (uuid == name_or_uuid);
    }
    return !name.empty() && (name == name_or_uuid.getName());
  }

  std::string NameAndUuid::toString() const
  {
    if(hasName()) return name;
    return Uuid::toString();
  }

}  // namespace NamingScheme
