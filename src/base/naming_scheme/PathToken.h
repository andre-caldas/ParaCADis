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

#ifndef NamingScheme_PathToken_H
#define NamingScheme_PathToken_H

#include "Uuid.h"

#include <base/xml/streams_fwd.h>

#include <string>

namespace NamingScheme
{

  /**
   * Each token in a path is a name or uuid, represented by PathToken.
   */
  class PathToken
  {
  private:
    Uuid        uuid;
    std::string name;

  public:
    PathToken(PathToken&&)                 = default;
    PathToken(const PathToken&)            = default;
    PathToken& operator=(PathToken&&)      = default;
    PathToken& operator=(const PathToken&) = default;

    PathToken(auto uuid) : uuid(uuid) {}
    PathToken(std::string name_or_uuid);

    std::string        toString() const;
    const std::string& getName() const { return name; }
    Uuid               getUuid() const { return uuid; }

    bool isName() const { return !uuid.isValid(); }
    bool isUuid() const { return uuid.isValid(); }

    operator Uuid::uuid_type() const { return uuid; }
    operator std::string() const { return toString(); }

    void             serialize(Xml::Writer& writer) const noexcept;
    static PathToken unserialize(Xml::Reader& reader);
  };

  struct PathToken_Tag : Xml::XmlTag {
    std::string_view getName() const override { return "PathToken"; }
  };

}  // namespace NamingScheme

#endif
