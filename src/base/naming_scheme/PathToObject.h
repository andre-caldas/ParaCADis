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

#ifndef NamingScheme_PathToObject_H
#define NamingScheme_PathToObject_H

#include "NameAndUuid.h"

#include <base/expected_behaviour/SharedPtr.h>
#include <base/xml/streams.h>

#include <deque>
#include <initializer_list>

namespace NamingScheme
{

  class Exporter;

  /**
   * @brief The list of tokens that composes a path.
   * This object cannot be resolved, but it can be appended
   * to a @PathToObject instance.
   */
  class ListOfPathTokens
  {
  protected:
    std::deque<NameOrUuid> tokens;

  public:
    ListOfPathTokens(ListOfPathTokens&&)                 = default;
    ListOfPathTokens(const ListOfPathTokens&)            = default;
    ListOfPathTokens& operator=(const ListOfPathTokens&) = default;
    ListOfPathTokens& operator=(ListOfPathTokens&&)      = default;

    ListOfPathTokens(std::initializer_list<NameOrUuid> init);
    virtual ~ListOfPathTokens() = default;

    std::string pathString() const;

    ListOfPathTokens& operator<<(NameOrUuid extra_token);
    ListOfPathTokens& operator<<(ListOfPathTokens extra_tokens);

    void                    serialize(Xml::Writer& writer) const;
    static ListOfPathTokens unserialize(Xml::Reader& reader);
  };


  /**
   * @brief A path to an object is more precisely,
   * a reference to a member of some object.
   * It is composed of:
   * 1. A url path to a document.
   * 2. A @class Uuid that identifies a root shared @class Exporter.
   * 3. A sequence of @class NameOrUuid that identifies the path
   *    to the referenced entity.
   *
   * Instances of the @class PathToObject are not aware of the type
   * of variable they point to.
   *
   * @attention This is internally used by @class ReferenceTo<T>,
   * and also used by the python bindings so we do not need
   * to create one binding for each possible @class ReferenceTo<T>.
   * Usually you should use @class ReferenceTo<T>, instead.
   */
  class PathToObject : private ListOfPathTokens
  {
  protected:
    WeakPtr<Exporter> root_weak_ptr;  ///< Try first.
    Uuid              root_uuid;      ///< Try second.
    std::string       root_url;       ///< Try this third. (not implemented)

  public:
    PathToObject(PathToObject&&)                 = default;
    PathToObject(const PathToObject&)            = default;
    PathToObject& operator=(const PathToObject&) = default;
    PathToObject& operator=(PathToObject&&)      = default;

    /**
     * A root object and a list of tokens.
     * @example `PathToObject(root, {"geometries", "second_line"});`
     */
    /// @{
    PathToObject(const SharedPtr<Exporter>& root, ListOfPathTokens tokens = {});
    PathToObject(Uuid root_uuid, ListOfPathTokens tokens = {});
    PathToObject(std::string root_url, ListOfPathTokens tokens = {});
    /// @}

    virtual ~PathToObject() = default;

    void                serialize(Xml::Writer& writer) const;
    static PathToObject unserialize(Xml::Reader& reader);

    PathToObject operator+(NameOrUuid extra_token) const;
    PathToObject operator+(ListOfPathTokens extra_tokens) const;
  };

}  // namespace NamingScheme

#endif
