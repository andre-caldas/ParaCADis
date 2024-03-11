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
    tokens.emplace_back(std::move(extra_path));
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
      : root_uuid(uuid), tokens(std::move(tokens)
  {
  }

  PathToObject::PathToObject(
      std::shared_ptr<Exporter> root, ListOfPathTokens tokens)
      : root_uuid(root->getUuid()), root_weak_ptr(root), tokens(std::move(tokens))
  {
  }


  ListOfPathTokens PathToObject::operator+(NameOrUuid extra_token) const
  {
    ListOfPathTokens result(*this);
    result << std::move(extra_token);
    return result;
  }

  PathToObject::lock_type PathToObject::getLock() const
  {
    // First we try the root_weak_ptr.
    auto last_object = root_weak_ptr.lock();
    // If it is not valid, we try to find the uuid in the global dictionary.
    if (!last_object) {
      last_object = Exporter::getSharedPtr(root_uuid);
    }

    if (!last_object) {
      throw ExceptionNoUuid(root_uuid, "Root object does not exist!");
    }

    // TODO: Implement different "cache expire" policies.
    lock_type lock{last_object: last_object, remaining_tokens: tokens};
    while (lock.remaining_tokens) {
      try {
        lock.last_object = lock.last_object->resolve<Exporter>(
            lock.last_object, lock.remaining_tokens);
      } catch (ExceptionNoExport&) {
        // Current object is not chainable.
        return lock;
      }

      auto previous_it = lock.remaining_tokens_start;

      if (lock.remaining_tokens_start == previous_it) {
        FC_THROWM(
            Base::RuntimeError,
            "Object's path resolution is not consuming tokens. Path: '"
                << pathString() << "'. This is a BUG!");
      }
    }
    return lock;
  }

  void PathToObject::serialize(Base::Writer& writer) const
  {
    writer.Stream() << writer.ind() << "<PathToObject>" << std::endl;
    writer.incInd();
    writer.Stream() << writer.ind() << "<root_uuid>";
    writer.Stream() << root_uuid.toString() << "</root_uuid>" << std::endl;
    for (auto token: tokens) {
      // TODO: escape token.getText().
      writer.Stream() << writer.ind() << "<NameOrUuid>";
      writer.Stream() << token.getText() << "</NameOrUuid>" << std::endl;
    }
    writer.decInd();
    writer.Stream() << writer.ind() << "</PathToObject>" << std::endl;
  }

  PathToObject PathToObject::unserialize(Base::XMLReader& reader)
  {
    reader.readElement("PathToObject");
    reader.readElement("root_uuid");
    auto root = Exporter::getWeakPtr(reader.getCharacters()).lock();
    if (!root) {
      FC_THROWM(
          ReferenceError,
          "Root element does not exist when unserializing RferenceTo: '"
              << reader.getCharacters() << "'");
    }
    PathToObject result{root};
    while (!reader.testEndElement("PathToObject")) {
      reader.readElement("NameOrUuid");
      result.tokens.push_back(NameAndUuid(reader.getCharacters()));
    }
    return result;
  }

}  // namespace NamingScheme

