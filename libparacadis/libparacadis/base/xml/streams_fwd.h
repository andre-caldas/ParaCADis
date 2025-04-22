// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2024 André Caldas <andre.em.caldas@gmail.com>            *
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

#ifndef Xml_streams_fwd_H
#define Xml_streams_fwd_H

#include <string_view>
#include <unordered_map>

namespace Xml
{

  class Reader;
  class Writer;

  class XmlTag
  {
  public:
    using attribute_list = std::unordered_map<std::string_view, const XmlTag&>;
    using subtag_list    = std::unordered_map<std::string_view, const XmlTag&>;

    virtual std::string_view      getName() const = 0;
    virtual const attribute_list& getAttributesAndDefaults() const;
    virtual const subtag_list&    getSubTags() const;

    virtual std::size_t getMinRepetitions() const { return 1; }
    virtual std::size_t getMaxRepetitions() const { return 1; }
    virtual ~XmlTag() = default;
  };

}  // namespace Xml

#endif
