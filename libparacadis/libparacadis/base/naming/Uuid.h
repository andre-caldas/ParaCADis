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

#ifndef NamingScheme_Uuid_H
#define NamingScheme_Uuid_H

#include <libparacadis/base/xml/streams_fwd.h>

#include <string>
#include <cassert>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace std
{
  template<>
  struct hash<boost::uuids::uuid> {
    size_t operator()(const boost::uuids::uuid& uuid) const noexcept;
  };
}

namespace Naming
{

  class Uuid
  {
  public:
    using uuid_type = boost::uuids::uuid;
    uuid_type uuid;

    Uuid();
    Uuid(int i);
    Uuid(uuid_type _uuid) : uuid(_uuid) {}
    Uuid(std::string_view uuid_str);
    virtual ~Uuid() = default;

    bool isValid() const;
    static bool isValid(std::string_view uuid_str);

    constexpr uuid_type getUuid() const { return uuid; }
    constexpr operator uuid_type() const { return uuid; }

    std::string toString() const { return boost::uuids::to_string(uuid); }
    operator std::string() const { return toString(); }

    bool operator==(const Uuid& other) const { return uuid == other.uuid; }

    void        serialize(Xml::Writer& writer) const noexcept;
    static Uuid unserialize(Xml::Reader& reader);
  };

  struct Uuid_Tag : Xml::XmlTag {
    std::string_view getName() const override { return "Uuid"; }
  };

}  // namespace Naming

#endif
