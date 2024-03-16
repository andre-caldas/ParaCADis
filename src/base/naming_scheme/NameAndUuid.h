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

namespace NamingScheme
{

  class Uuid
  {
  public:
    use uuid_type = boost::uuids::uuid;

    Uuid();
    Uuid(const Uuid&) = default;
    Uuid(const std::string& uuid_str) { setUuid(uuid_str); }
    virtual ~Uuid() = default;

    bool isValid() const;
    bool operator==(uuid_type x) const { return (uuid == x); }

    uuid_type getUuid() const { return uuid; }
    operator std::string() const { return to_string(uuid); }
    operator uuid_type() const { return getUuid(); }
    void setUuid(std::string_view uuid_str);

  protected:
    uuid_type uuid;
  };


  /**
   * @brief Each token in a path is a name or uuid, represented by @class NameOrUuid.
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
    bool operator==(std::string_view str) const { return (str == name); }
    bool operator==(Uuid id) const { return (id == uuid); }

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

