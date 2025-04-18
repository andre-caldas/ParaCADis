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

using namespace Document;
using namespace NamingScheme;

SharedPtr<Container> Container::deepCopy() const
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
SharedPtr<ExporterCommon> Container::getElement(uuid_type uuid) const
{
  { // gate scope.
    Threads::ReaderGate gate{non_containers};
    auto it = gate->find(uuid);
    if (it != gate->end()) {
      return it->second;
    }
  }
  return getContainer(uuid);
}

SharedPtr<Container> Container::getContainer(uuid_type uuid) const
{
  Threads::ReaderGate gate{containers};
  auto it = gate->find(uuid);
  if (it != gate->end()) {
    return it->second;
  }
  throw Exception::ElementNotInContainer(uuid, *this);
}

/*
 * Add elements.
 */
void Container::addElement(SharedPtr<ExporterCommon> element)
{
  auto ptr = element.cast<Container>();
  if (ptr) {
    addContainer(ptr);
    return;
  }

  Threads::WriterGate gate{non_containers};
  if (gate->contains(element->getUuid())) {
    throw Exception::ElementAlreadyInContainer(*element, *this);
  }
  gate->emplace(element->getUuid(), element);
  add_non_container_sig.emit_signal(std::move(element));
}

void Container::addContainer(SharedPtr<Container> container)
{
  Threads::WriterGate gate{containers};
  if (gate->contains(container->getUuid())) {
    throw Exception::ElementAlreadyInContainer(*container, *this);
  }
  gate->emplace(container->getUuid(), container);
  add_container_sig.emit_signal(std::move(container));
}


/*
 * Remove elements.
 */
void Container::removeElement(SharedPtr<ExporterCommon> element)
{
  auto ptr = element.cast<Container>();
  if (ptr) {
    removeContainer(ptr);
    return;
  }

  removeElement(element->getUuid());
}

SharedPtr<ExporterCommon> Container::removeElement(uuid_type uuid)
{
  { // Gate scope
  Threads::WriterGate gate{non_containers};
    auto nh = gate->extract(uuid);
    if (nh) {
      remove_non_container_sig.emit_signal(nh.mapped());
      return std::move(nh.mapped());
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
    remove_container_sig.emit_signal(nh.mapped());
    return std::move(nh.mapped());
  }
  throw Exception::ElementNotInContainer(uuid, *this);
}


/*
 * Move element from one container to the other *atomically*.
 */
void Container::moveElementTo(uuid_type uuid, const SharedPtr<Container>& to)
{
  { // lock context
    Threads::WriterGate gate{non_containers, to->non_containers};
    if(gate[to->non_containers].contains(uuid)) {
      throw Exception::ElementAlreadyInContainer(uuid, *to);
    }

    if(gate[non_containers].contains(uuid)) {
      auto e = gate[non_containers].extract(uuid);
      auto e_shptr = e.mapped();
      gate[to->non_containers].insert(std::move(e));
      move_non_container_sig.emit_signal(std::move(e_shptr), to);
      return;
    }
  }
  moveContainerTo(uuid, to);
}

void Container::moveContainerTo(uuid_type uuid, const SharedPtr<Container>& to)
{
  Threads::WriterGate lock{containers, to->containers};
  if(lock[to->containers].contains(uuid)) {
    throw Exception::ElementAlreadyInContainer(uuid, *to);
  }

  if(lock[containers].contains(uuid)) {
    auto e = lock[containers].extract(uuid);
    auto e_shptr = e.mapped();
    lock[to->containers].insert(std::move(e));
    move_container_sig.emit_signal(std::move(e_shptr), to);
    return;
  }
  throw Exception::ElementNotInContainer(uuid, *to);
}


bool Container::contains(uuid_type uuid) const
{
  Threads::ReaderGate gate{containers, non_containers};
  if(gate[containers]    .contains(uuid)) { return true; }
  if(gate[non_containers].contains(uuid)) { return true; }

  return false;
}

bool Container::contains(const ExporterCommon& element) const
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

SharedPtr<DeferenceableCoordinates>
Container::getCoordinates() const
{
  return coordinate_system.getSharedPtr();
}

SharedPtr<DeferenceableCoordinates>
Container::setCoordinates(SharedPtr<DeferenceableCoordinates> coordinates)
{
  auto self = getSelfShared<Container>();

  auto old = coordinate_system.getSharedPtr();
  if(old) {
    old->getChangedSignal()
        .removeProxy(&coordinate_modified_sig);
  }

  coordinates->getChangedSignal()
              .setProxy(std::move(self), &Container::coordinate_modified_sig);
  return coordinate_system.setSharedPtr(std::move(coordinates));
}


SharedPtr<ExporterCommon>
Container::resolve_shared(token_iterator& tokens, ExporterCommon*)
{
  if(!tokens) {
    return {};
  }

  auto& token = tokens.front();
  Threads::ReaderGate gate{containers, non_containers};

  // Uuid.
  if(token.isUuid()) {
    auto it_n = gate[non_containers].find(token);
    if(it_n != gate[non_containers].end()) {
      tokens.advance(1);
      return it_n->second;
    }

    auto it_c = gate[containers].find(token);
    if(it_c != gate[containers].end())
    {
      tokens.advance(1);
      return it_c->second;
    }

    return {};
  }

  // Name.
  for(auto& [uuid, ptr] : non_containers)
  {
    if(ptr->getName() == token.getName())
    {
      tokens.advance(1);
      return ptr;
    }
  }

  for(auto& [uuid, ptr] : containers)
  {
    if(ptr->getName() == token.getName())
    {
      tokens.advance(1);
      return ptr;
    }
  }

  return {};
}

SharedPtr<Container>
Container::resolve_shared(token_iterator& tokens, Container*)
{
  if(!tokens)
  {
    return {};
  }

  auto& token = tokens.front();
  if (token.isUuid())
  {
    auto it = containers.find(token);
    if(it != containers.end())
    {
      tokens.advance(1);
      return it->second;
    }
    return {};
  }

  for(auto& [uuid, ptr] : containers)
  {
    if(ptr->getName() == token.getName())
    {
      tokens.advance(1);
      return ptr;
    }
  }

  return {};
}

SharedPtr<DeferenceableCoordinates>
Container::resolve_shared(token_iterator& tokens, DeferenceableCoordinates*)
{
  if(!tokens)
  {
    return {};
  }

  if(tokens.front() == "axis")
  {
    tokens.advance(1);
    return coordinate_system.getSharedPtr();
  }
  return {};
}
