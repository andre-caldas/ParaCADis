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
#include <base/threads/locks/MutexesWithPivot.h>
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
    using uuid_type = NamingScheme::Uuid::uuid_type;

    std::unique_ptr<Container> deepCopy() const;
    std::unique_ptr<NamingScheme::ExporterBase> deepCopyExporter() const override
    { return deepCopy(); }

    std::string toString() const override;

    SharedPtr<ExporterBase> getElement(uuid_type uuid) const;
    SharedPtr<Container> getContainer(uuid_type uuid) const;

    template<std::convertible_to<const Container&> C>
    void addElement(SharedPtr<C> c) { addContainer(std::move(c)); }
    void addElement(SharedPtr<ExporterBase> element);
    void addContainer(SharedPtr<Container> container);

    template<std::convertible_to<const Container&> C>
    void removeElement(SharedPtr<C> c) { removeContainer(std::move(c)); }
    void removeElement(SharedPtr<ExporterBase> element);
    SharedPtr<ExporterBase> removeElement(uuid_type uuid);
    void removeContainer(SharedPtr<Container> container);
    SharedPtr<Container> removeContainer(uuid_type uuid);

    /**
     * Moves element from one container to the other, **atomically**.
     *
     * @attention If the move operation fails, everything should just
     * remain just as if the moveElement() method was not called.
     */
    /// @{
    void moveElementTo(uuid_type uuid, Container& to);
    void moveContainerTo(uuid_type uuid, Container& to);
    /// @}

    bool contains(const ExporterBase& element) const;
    bool contains(const Container& container) const;
    bool contains(std::string_view name) const;
    bool contains(uuid_type uuid) const;

    SharedPtr<ExporterBase>
    resolve_share(token_iterator& tokens, ExporterBase* = nullptr) override;
    SharedPtr<Container>
    resolve_share(token_iterator& tokens, Container* = nullptr) override;
    DeferenceableCoordinateSystem*
    resolve_ptr(token_iterator& tokens, DeferenceableCoordinateSystem* = nullptr) override;

  private:
    template<typename Key, typename Val>
    using UnorderedMap = Threads::SafeStructs::ThreadSafeUnorderedMap<Key, Val>;

    // Data protected by mutexes.
    UnorderedMap<uuid_type, SharedPtr<NamingScheme::ExporterBase>> non_containers;
    UnorderedMap<uuid_type, SharedPtr<Container>> containers;
    DeferenceableCoordinateSystem                 coordinate_system;

    /**
     * This is the pivot mutex. No one should use it directly.
     * @attention A SharedLock on it is not what you expect.
     * Use `mutex` instead.
     * @see MutexesWithPivot.
     */
    mutable Threads::MutexesWithPivot<
        Threads::MutexData, Threads::MutexData, Threads::MutexData>
        mutex{non_containers.getMutexData(),
              containers.getMutexData(),
              coordinate_system.getMutexData()};

  public:
    using mutex_data_t = decltype(mutex);
    constexpr mutex_data_t& getMutexData() const { return mutex; }
    constexpr operator mutex_data_t&() const { return mutex; }
  };

  static_assert(Threads::C_MutexHolder<Container>, "A container is a C_MutexHolder.");

}  // namespace DocumentTree

#endif
