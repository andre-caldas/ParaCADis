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

#include "Container.h"

#include "exceptions.h"

#include <base/threads/safe_structs/ThreadSafeMap.h>

namespace DocumentTree
{

  std::string Container::toString() const
  {
    return std::format("Container ({})", getName());
  }

  void Container::addElement(SharedPtr<ExporterBase> element)
  {
    auto ptr = element.cast<Container>();
    if (ptr) {
      addContainer(ptr);
      return;
    }

    auto gate = non_containers.getWriterGate();
    if (gate->contains(element->getUuid())) {
      throw Exception::ElementAlreadyInContainer(element, sharedFromThis<Container>());
    }
    gate->emplace(element->getUuid(), std::move(element));
  }

  void Container::addContainer(SharedPtr<Container> container)
  {
    auto gate = containers.getWriterGate();
    if (!gate->contains(container->getUuid())) {
      throw Exception::ElementAlreadyInContainer(container, sharedFromThis<Container>());
    }
    gate->emplace(container->getUuid(), std::move(container));
  }

  bool Container::contains(NamingScheme::Uuid::uuid_type uuid) const
  {
    {
      auto gate = containers.getReaderGate();
      if(gate->contains(uuid)) { return true; }
    }

    {
      auto gate = non_containers.getReaderGate();
      if(gate->contains(uuid)) { return true; }
    }

    return false;
  }

  bool Container::contains(const ExporterBase& element) const
  {
    return contains(element.getUuid());
  }

  bool Container::contains(const Container& container) const
  {
    auto gate = containers.getReaderGate();
    return gate->contains(container.getUuid());
  }

  bool Container::contains(std::string_view name) const
  {
    Threads::SharedLock c{containers};
    Threads::SharedLock n{non_containers};
    for (const auto& [uuid, exporter] : containers) {
      if (exporter->getName() == name) { return true; }
    }
    for (const auto& [uuid, exporter] : non_containers) {
      if (exporter->getName() == name) { return true; }
    }
    return false;
  }


  SharedPtr<ExporterBase>
  Container::resolve_share(token_iterator& tokens, ExporterBase*)
  {
    if(!tokens)
    {
      return {};
    }

    auto result = Chainables::resolve_share(tokens);
    if(result)
    {
      return result;
    }

    auto& token = tokens.front();
    if (token.isUuid())
    {
      auto it = non_containers.find(token);
      if(it != non_containers.end())
      {
        return it->second;
      }
      return {};
    }

    for(auto& [uuid, ptr] : non_containers)
    {
      if(ptr->getName() == token.getName())
      {
        return ptr;
      }
    }

    return {};
  }

  SharedPtr<Container>
  Container::resolve_share(token_iterator& tokens, Container*)
  {
    auto& token = tokens.front();
    if (token.isUuid())
    {
      auto it = containers.find(token);
      if(it != containers.end())
      {
        return it->second;
      }
      return {};
    }

    for(auto& [uuid, ptr] : containers)
    {
      if(ptr->getName() == token.getName())
      {
        return ptr;
      }
    }

    return {};
  }

  DeferenceableCoordinateSystem*
  Container::resolve_ptr(token_iterator& tokens, DeferenceableCoordinateSystem*)
  {
    if(!tokens)
    {
      return nullptr;
    }

    if(tokens.front() == "axis")
    {
      tokens.advance(1);
      return &coordinate_system;
    }
    return nullptr;
  }

}  // namespace DocumentTree
