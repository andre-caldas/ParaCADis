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

#ifndef DocumentTree_Container_H
#define DocumentTree_Container_H

#include <base/expected_behaviour/SharedPtr.h>
#include <base/geometric_primitives/CoordinateSystem.h>
#include <base/naming_scheme/Chainables.h>
#include <base/naming_scheme/Exporter.h>
#include <base/naming_scheme/IExport.h>
#include <base/threads/safe_structs/ThreadSafeMap.h>

#include <concepts>

namespace DocumentTree
{

  class Container
      : public NamingScheme::ExporterBase
      , public NamingScheme::IExport<Container>
      , public NamingScheme::IExport<DeferenceableCoordinateSystem>
      , public NamingScheme::Chainables<Container, DeferenceableCoordinateSystem>
  {
    using ExporterBase = NamingScheme::ExporterBase;
    template<typename T>
    using IExport = NamingScheme::IExport<T>;

  public:
    std::string toString() const override;

    template<std::convertible_to<const Container&> C>
    void addElement(SharedPtr<C> c) { addContainer(std::move(c)); }
    void addElement(SharedPtr<ExporterBase> element);
    void addContainer(SharedPtr<Container> container);

    bool contains(NamingScheme::Uuid::uuid_type uuid) const;
    bool contains(const ExporterBase& element) const;
    bool contains(const Container& container) const;
    bool contains(std::string_view name) const;

    SharedPtr<ExporterBase>
    resolve_share(token_iterator& tokens, ExporterBase* = nullptr) override;
    SharedPtr<Container>
    resolve_share(token_iterator& tokens, Container* = nullptr) override;
    DeferenceableCoordinateSystem*
    resolve_ptr(token_iterator& tokens, DeferenceableCoordinateSystem* = nullptr) override;

  private:
    using uuid_type = NamingScheme::Uuid::uuid_type;
    template<typename Key, typename Val>
    using UnorderedMap = Threads::SafeStructs::ThreadSafeUnorderedMap<Key, Val>;

    // TODO: do we need to use the same mutex?
    // It is safer, for sure.
    // Maybe we could enforce SharedLock of Exporter::mutex
    // before the ExclusiveLock or SharedLock for the "sub-container" mutex.
    // That would be theoretically less "blocking".
    UnorderedMap<uuid_type, SharedPtr<NamingScheme::ExporterBase>> non_containers{mutex};
    UnorderedMap<uuid_type, SharedPtr<Container>> containers{mutex};

    DeferenceableCoordinateSystem coordinate_system;
  };

  static_assert(Threads::C_MutexHolder<Container>, "A container is a C_MutexHolder.");

}  // namespace DocumentTree

#endif
