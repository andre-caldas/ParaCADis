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

#include "NameAndUuid.h"
#include "exceptions.h"

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

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
    // A static POD is zero initialized.
    static boost::uuids::uuid zero_uuid;
  }

  Uuid::Uuid()
      : uuid(random_generator())
  {
  }

  void Uuid::isValid() const
  {
    return uuid != zero_uuid;
  }

  void Uuid::setUuid(std::string_view uuid)
  {
    try {
      uuid = string_generator(uuid);
      assert(isValid);
    } catch (const std::runtime_error&) {
      // Zero-initialize.
      uuid = uuid_type();
      assert(!isValid());
    }
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

  /*
   * NameAnduuid
   * ===========
   */

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
      throw ExceptionInvalidName();
    }
    name = std::move(name_str);
  }

  bool NameAndUuid::pointsToMe(NameOrUuid& name_or_uuid) const
  {
    if (name_or_uuid.isUuid()) {
      return (uuid == name_or_uuid.uuid);
    }
    return !name.empty() && (name == name_or_uuid.name);
  }

  std::string NameAndUuid::toString() const
  {
    return name.empty() ? to_string(uuid) : name;
  }

}  // namespace NamingScheme

