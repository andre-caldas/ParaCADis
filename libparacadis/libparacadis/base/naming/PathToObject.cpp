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

#include "PathToObject.h"

#include "Exporter.h"
#include "PathToken.h"

#include <libparacadis/base/exceptions.h>

using namespace Naming;

ListOfPathTokens::ListOfPathTokens(std::initializer_list<PathToken> init) : tokens(init)
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

ListOfPathTokens& ListOfPathTokens::operator<<(PathToken extra_token)
{
  tokens.emplace_back(std::move(extra_token));
  return *this;
}

ListOfPathTokens& ListOfPathTokens::operator<<(ListOfPathTokens extra_tokens)
{
  // TODO: change to "append_range" when available.
  // tokens.append_range(std::move(extra_tokens.tokens));
  auto end   = std::make_move_iterator(extra_tokens.tokens.end());
  auto begin = std::make_move_iterator(extra_tokens.tokens.begin());
  tokens.insert(tokens.cend(), begin, end);
  return *this;
}


void ListOfPathTokens::serialize(Xml::Writer& writer) const noexcept
{
  writer.reportException(Exception::NotImplemented{});
}

ListOfPathTokens ListOfPathTokens::unserialize(Xml::Reader&)
{
  throw Exception::NotImplemented();
}

/*
 * PathToObject
 * ============
 */

PathToObject::PathToObject(Uuid _root_uuid, ListOfPathTokens tokens)
    : root_uuid(std::move(_root_uuid))
    , list_of_tokens(std::move(tokens))
{
}

PathToObject::PathToObject(const SharedPtr<ExporterCommon>& root, ListOfPathTokens tokens)
    : root_weak_ptr(root.getWeakPtr())
    , root_uuid(root->getUuid())
    , list_of_tokens(std::move(tokens))
{
}

PathToObject::PathToObject(std::string _root_url, ListOfPathTokens tokens)
    : root_url(_root_url)
    , list_of_tokens(std::move(tokens))
{
  throw ::Exception::NotImplemented();
}

PathToObject PathToObject::operator+(PathToken extra_token) const
{
  return *this + ListOfPathTokens({extra_token});
}

PathToObject PathToObject::operator+(ListOfPathTokens extra_tokens) const
{
  ListOfPathTokens path(list_of_tokens);
  path << std::move(extra_tokens);
  PathToObject result(root_uuid, std::move(path));
  result.root_weak_ptr = root_weak_ptr;
  result.root_url      = root_url;
  return result;
}


SharedPtr<ExporterCommon> PathToObject::getRoot() const
{
  auto result = root_weak_ptr.lock();
  if(result) {
    return result;
  }

  assert(false);
}


void PathToObject::serialize(Xml::Writer& writer) const noexcept
{
  auto tag_writer = writer.newTag(PathToObject_Tag{});
  try {
    root_uuid.serialize(writer);
    for (const auto& token: getTokens()) { token.serialize(writer); }
  } catch (const std::exception& s) {
    tag_writer.reportException(s);
  }
}

PathToObject PathToObject::unserialize(Xml::Reader& reader)
{
  auto root_uuid    = Uuid::unserialize(reader);
  auto list_of_tags = ListOfPathTokens::unserialize(reader);
  return PathToObject(std::move(root_uuid), std::move(list_of_tags));
}

const PathToObject_Tag::subtag_list& PathToObject_Tag::getSubTags() const
{
  assert(false);
}
