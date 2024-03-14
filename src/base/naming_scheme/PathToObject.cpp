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

#include "Exporter.h"
#include "PathToObject.h"

namespace NamingScheme
{

  /*
   * ListOfPathTokens
   * ================
   */

  ListOfPathTokens::ListOfPathTokens(std::initializer_list<NameOrUuid> init)
      : tokens(init)
  {
  }

  std::string ListOfPathTokens::pathString() const
  {
    std::string result;
    for (const auto& token: tokens) {
      result += '.';
      result += token;
    }
    return result;
  }

  ListOfPathTokens& ListOfPathTokens::operator<<(NameOrUuid extra_token)
  {
    tokens.emplace_back(std::move(extra_token));
    return *this;
  }

  ListOfPathTokens& ListOfPathTokens::operator<<(ListOfPathTokens extra_tokens)
  {
    tokens.append_range(std::move(extra_tokens.tokens));
    return *this;
  }


  void ListOfPathTokens::serialize(Files::XmlWriter& writer) const
  {
    throw NotImplemented();
  }

  ListOfPathTokens ListOfPathTokens::unserialize(Files::XmlReader& reader)
  {
    throw NotImplemented();
  }

  /*
   * PathToObject
   * ============
   */

  PathToObject::PathToObject(const Uuid& uuid, ListOfPathTokens tokens)
      : root_uuid(uuid), tokens(std::move(tokens))
  {
  }

  PathToObject::PathToObject(SharedPtr<Exporter> root, ListOfPathTokens tokens)
      : root_weak_ptr(root), root_uuid(root->getUuid()), tokens(std::move(tokens))
  {
  }

  PathToObject PathToObject::operator+(NameOrUuid extra_token) const
  {
    ListOfPathTokens path(*this);
    path << std::move(extra_token);
    PathToObject result(root_uuid, std::move(path));
    result.root_weak_ptr = root_weak_ptr;
    result.url           = url;
    return result;
  }

  PathToObject PathToObject::operator+(ListOfPathTokens extra_tokens) const
  {
    ListOfPathTokens path(*this);
    path << std::move(extra_tokens);
    PathToObject result(root_uuid, std::move(path));
    result.root_weak_ptr = root_weak_ptr;
    result.url           = url;
    return result;
  }


  void PathToObject::serialize(Files::XmlWriter& writer) const
  {
    auto sentinel = writer.newTag("PathToObject");
    try {
      root_uuid.serialize(writer);
      for (const auto& token: tokens) {
        token.serialize(writer);
      }
    } catch (Files::XmlWriter::SerializeError s) {
      writer.reportError(s);
    }
  }

  PathToObject PathToObject::unserialize(Files::XmlReader& reader)
  {
    auto sentinel = reader.readOpenElement("PathToObject");
    auto root_uuid = Uuid::unserialize();
    auto list_of_tags = ListOfPathTokens::unserialize();
    return PathToObject(std::move(root_uuid), std::move(list_of_tags));
  }

}  // namespace NamingScheme

