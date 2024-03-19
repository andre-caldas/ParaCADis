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

#ifndef NamingScheme_NameAndUuid_H
#define NamingScheme_NameAndUuid_H

#include <string>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace std
{
  template<>
  struct hash<boost::uuids::uuid>
  {
    size_t operator()(const boost::uuids::uuid& uuid) const noexcept;
  };
}

namespace NamingScheme
{

  class Uuid
  {
  public:
    using uuid_type = boost::uuids::uuid;
    const uuid_type uuid;

    Uuid();
    Uuid(int i);
    Uuid(const Uuid&) = default;
    Uuid(std::string_view uuid_str);
    virtual ~Uuid() = default;

    bool isValid() const;

    uuid_type getUuid() const { return uuid; }
    operator uuid_type() const { return uuid; }
    operator std::string() const { return boost::uuids::to_string(uuid); }
  };


  /**
   * Each token in a path is a name or uuid, represented by NameOrUuid.
   */
  class NameOrUuid
  {
  private:
    Uuid        uuid;
    std::string name;

  public:
    NameOrUuid(const NameOrUuid&) = default;
    NameOrUuid(NameOrUuid&&)      = default;
    NameOrUuid(auto uuid) : uuid(uuid) {}
    NameOrUuid(std::string name_or_uuid);

    const std::string& getName() const { return name; }
    Uuid getUuid() const { return uuid; }
    operator Uuid::uuid_type() const { return uuid; }

    bool isName() const { return !uuid.isValid(); }
    bool isUuid() const { return uuid.isValid(); }
  };


  /**
   * @brief Object held by things that have an UUID and possibly a name.
   */
  class NameAndUuid : private Uuid
  {
  private:
    std::string name;

  public:
    using Uuid::Uuid;
    using Uuid::operator Uuid::uuid_type;

    /**
     * @brief Names cannot look like a UUID.
     */
    static bool isValidName(std::string_view name_str);

    /**
     * @brief If @a name_str is a valid name, set it.
     * Otherwise, throws `ExceptionInvalidName()`.
     */
    void setName(std::string name_str);
    void unsetName() { setName(""); }

    /**
     * @brief Verifies if @a name_or_uuid refers to this object
     */
    bool pointsToMe(const NameOrUuid& name_or_uuid) const;

    std::string toString() const;
    operator std::string() const { return toString(); }
    bool               hasName() const { return !name.empty(); }
    const std::string& getName() const { return name; }
  };

}  // namespace NamingScheme

#endif  // NamingScheme_NameAndUuid_H
