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

using namespace DocumentTree;

std::unique_ptr<Container> Container::deepCopy() const
{
  throw ::Exception::NotImplemented();
}

std::string Container::toString() const
{
  return std::format("Container ({})", getName());
}


/*
 * Get element.
 */
SharedPtr<ExporterBase> Container::getElement(uuid_type uuid) const
{
  { // gate scope.
    Threads::ReaderGate gate{non_containers};
    if (gate->contains(uuid)) {
      return gate->at(uuid);
    }
  }
  return getContainer(uuid);
}

SharedPtr<Container> Container::getContainer(uuid_type uuid) const
{
  Threads::ReaderGate gate{containers};
  if (gate->contains(uuid)) {
    return gate->at(uuid);
  }
  throw Exception::ElementNotInContainer(uuid, *this);
}

/*
 * Add elements.
 */
void Container::addElement(SharedPtr<ExporterBase> element)
{
  auto ptr = element.cast<Container>();
  if (ptr) {
    addContainer(ptr);
    return;
  }

  Threads::WriterGate gate{non_containers};
  if (gate->contains(element->getUuid())) {
    throw Exception::ElementAlreadyInContainer(element, *this);
  }
  gate->emplace(element->getUuid(), std::move(element));
}

void Container::addContainer(SharedPtr<Container> container)
{
  Threads::WriterGate gate{containers};
  if (gate->contains(container->getUuid())) {
    throw Exception::ElementAlreadyInContainer(container, *this);
  }
  gate->emplace(container->getUuid(), std::move(container));
}


/*
 * Remove elements.
 */
void Container::removeElement(SharedPtr<ExporterBase> element)
{
  auto ptr = element.cast<Container>();
  if (ptr) {
    removeContainer(ptr);
    return;
  }

  removeElement(element->getUuid());
}

SharedPtr<ExporterBase> Container::removeElement(uuid_type uuid)
{
  { // Gate scope
  Threads::WriterGate gate{non_containers};
    auto nh = gate->extract(uuid);
    if (nh) {
      return nh.mapped();
    }
  }
  return removeContainer(uuid);
}

void Container::removeContainer(SharedPtr<Container> container)
{
  removeContainer(container->getUuid());
}

SharedPtr<Container> Container::removeContainer(uuid_type uuid)
{
  Threads::WriterGate gate{containers};
  auto nh = gate->extract(uuid);
  if (nh) {
    return nh.mapped();
  }
  throw Exception::ElementNotInContainer(uuid, *this);
}


/*
 * Move element from one container to the other *atomically*.
 */
void Container::moveElementTo(uuid_type uuid, Container& to)
{
  { // lock context
    Threads::WriterGate gate{non_containers, to.non_containers};
    if(gate[to.non_containers].contains(uuid)) {
      throw Exception::ElementAlreadyInContainer(uuid, to);
    }

    if(gate[non_containers].contains(uuid)) {
      auto e = gate[non_containers].extract(uuid);
      gate[to.non_containers].insert(std::move(e));
    }
  }
  moveContainerTo(uuid, to);
}

void Container::moveContainerTo(uuid_type uuid, Container& to)
{
  Threads::WriterGate lock{containers, to.containers};
  if(lock[to.containers].contains(uuid)) {
    throw Exception::ElementAlreadyInContainer(uuid, to);
  }

  if(lock[containers].contains(uuid)) {
    auto e = lock[containers].extract(uuid);
    lock[to.containers].insert(std::move(e));
  }
  throw Exception::ElementNotInContainer(uuid, to);
}


bool Container::contains(uuid_type uuid) const
{
  Threads::ReaderGate gate{containers, non_containers};
  if(gate[containers]    .contains(uuid)) { return true; }
  if(gate[non_containers].contains(uuid)) { return true; }

  return false;
}

bool Container::contains(const ExporterBase& element) const
{
  return contains(element.getUuid());
}

bool Container::contains(const Container& container) const
{
  Threads::ReaderGate gate{containers};
  return gate->contains(container.getUuid());
}

bool Container::contains(std::string_view name) const
{
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
