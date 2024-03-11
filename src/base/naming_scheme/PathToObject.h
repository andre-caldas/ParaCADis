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

#include <deque>
#include <initializer_list>

namespace NamingScheme
{

  class NameOrUuid;
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

    void                    serialize(Files::XmlWriter& writer) const;
    static ListOfPathTokens unserialize(Files::XmlReader& reader);
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
    std::string             document_url;  // Not used yet!
    Uuid                    root_uuid;
    std::weak_ptr<Exporter> root_weak_ptr;

  public:
    PathToObject(PathToObject&&)                 = default;
    PathToObject(const PathToObject&)            = default;
    PathToObject& operator=(const PathToObject&) = default;
    PathToObject& operator=(PathToObject&&)      = default;

    /**
     * @brief Constructor to use with @class ListOfPathTokens or `{}`.
     * @example `PathToObject(root, {"geometries", "second_line"});`
     */
    PathToObject(std::shared_ptr<Exporter> root, ListOfPathTokens tokens);
    PathToObject(Uuid root_uuid, ListOfPathTokens tokens);

    virtual ~PathToObject() = default;

    /**
     * @brief References are *NOT* serialized with knowledge
     * of what is is the most derived class.
     * When unserializing, the application needs to know what specific
     * subclass must be instantiated.
     * Then, serialization is implemented as a static method of the derived class.
     * @param writer - stream to write to.
     */
    void                serialize(Files::XmlWriter& writer) const;
    static PathToObject unserialize(Files::XmlReader& reader);

    ListOfPathTokens operator+(NameOrUuid extra_token) const;

    /**
     * @brief The resolution mechanism resolves the token chain
     * up to the end of the chain, or up to the first non-chainable object.
     * The last found object is kept (using a shared_ptr) at "last_object".
     *
     * In the context of @class ReferenceTo<X>,
     * this last found object has to be of type:
     * * @class X, when there are no remaining tokens.
     * * @class IExport<X>, where there are tokens to be resolved.
     */
    struct lock_type {
      // TODO: Implement different lock_types for different "cache" policies.
      std::shared_ptr<Exporter>          last_object;
      std::ranges::subrange<token_range> remaining_tokens;
    };

    /**
     * @brief Resolves the reference until it finds a non @class Chainable
     * object, or until the chain is fully consumed.
     * @return A lock to the shared resource and a "list" of the remaining
     * tokens. That is, a @class lock_type.
     */
    lock_type getLock() const;
  };

}  // namespace NamingScheme

#endif  // BASE_Accessor_PathToObject_H

