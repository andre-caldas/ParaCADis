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

#ifndef Xml_streams_H
#define Xml_streams_H

#include "streams_fwd.h"

#include <cassert>
#include <iostream>
#include <vector>

namespace Xml
{

  /**
   * Parses XML and generates the corresponding data.
   *
   * @attention If no looping and no conditional is used (use readAllSubTags()),
   * then the XML Schema can also be generated.
   * For that, just make the unserialize() functions take a templated Reader.
   * Later, this Reader can be replaced by GenerateXsd.
   */
  class Reader
  {
  public:
    Reader(const XmlTag& _current_tag, std::istream& _stream)
        : current_tag(_current_tag)
        , stream(_stream)
    {
    }

    [[nodiscard]]
    Reader              readSubTag(const XmlTag& tag);
    std::vector<Reader> readAllSubTags();

    void reportException(const std::exception& e) const;

  private:
    const XmlTag& current_tag;
    std::istream& stream;
  };


  class Writer
  {
  public:
    Writer(const XmlTag& _current_tag, std::istream& _stream)
        : current_tag(_current_tag)
        , stream(_stream)
    { assert(false && "Not implemented."); }

    [[nodiscard]]
    Writer newTag(const XmlTag& tag) noexcept;
    [[noreturn]]
    void reportException(const std::exception& e);

  private:
    const XmlTag& current_tag;
    std::istream& stream;
  };

}  // namespace Xml

#endif
