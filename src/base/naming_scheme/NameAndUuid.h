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

#include "Uuid.h"

#include <base/xml/streams_fwd.h>

#include <string>

namespace NamingScheme
{

  class PathToken;

  /**
   * Object held by things that have an UUID and possibly a name.
   */
  class NameAndUuid
  {
  private:
    Uuid        uuid;
    std::string name;

  public:
    /**
     * Default constructor uses a random Uuid.
     */
    NameAndUuid() = default;

    /**
     * Uuid must be unique. So we forbid copy.
     */
    /// @{
    NameAndUuid(const NameAndUuid&)    = delete;
    void operator=(const NameAndUuid&) = delete;
    /// @}

    /**
     * On the other hand, we would like to be able to unserialize Uuid.
     */
    /// @{
    NameAndUuid(const Uuid& uuid);
    NameAndUuid(const Uuid& uuid, std::string name = {});
    /// @}

    /**
     * @brief Names cannot look like a UUID.
     */
    static bool isValidName(std::string_view name_str);

    /**
     * @brief If @a name_str is a valid name, set it.
     * Otherwise, throws `Exception::InvalidName()`.
     */
    void setName(std::string name_str);
    void unsetName() { setName(""); }

    /**
     * @brief Verifies if @a name_or_uuid refers to this object
     */
    bool pointsToMe(const PathToken& name_or_uuid) const;

    std::string toString() const;
    operator std::string() const { return toString(); }
    bool               hasName() const { return !name.empty(); }
    const std::string& getName() const { return name; }
    Uuid               getUuid() const { return uuid; }


    void               serialize(Xml::Writer& writer) const noexcept;
    static NameAndUuid unserialize(Xml::Reader& reader);
  };

  struct NameAndUuid_Tag : Xml::XmlTag {
    std::string_view getName() const override { return "NameAndUuid"; }
  };

}  // namespace NamingScheme

#endif  // NamingScheme_NameAndUuid_H
