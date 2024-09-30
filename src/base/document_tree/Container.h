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

#pragma once

#include <base/expected_behaviour/SharedPtr.h>
#include <base/geometric_primitives/CoordinateSystem.h>
#include <base/naming_scheme/Chainables.h>
#include <base/naming_scheme/Exporter.h>
#include <base/naming_scheme/IExport.h>
#include <base/threads/safe_structs/ThreadSafeMap.h>
#include <base/threads/message_queue/Signal.h>
#include <base/threads/message_queue/MutexSignal.h>

#include <concepts>
#include <ranges>

namespace Document
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
    std::unique_ptr<ExporterBase> deepCopyExporter() const override
    { return deepCopy(); }

    std::string toString() const override;

    SharedPtr<ExporterBase> getElement(uuid_type uuid) const;
    SharedPtr<Container> getContainer(uuid_type uuid) const;

//    template<std::convertible_to<const Container&> C>
//    void addElement(SharedPtr<C> c) { addContainer(std::move(c)); }
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
    void moveElementTo(uuid_type uuid, const SharedPtr<Container>& to);
    void moveContainerTo(uuid_type uuid, const SharedPtr<Container>& to);
    /// @}

    bool contains(const ExporterBase& element) const;
    bool contains(const Container& container) const;
    bool contains(std::string_view name) const;
    bool contains(uuid_type uuid) const;

    auto containersView() const
    {return std::ranges::subrange(containers); }
    auto nonContainersView() const
    { return std::ranges::subrange(non_containers); }


    SharedPtr<ExporterBase>
    resolve_shared(token_iterator& tokens, ExporterBase* = nullptr) override;
    SharedPtr<Container>
    resolve_shared(token_iterator& tokens, Container* = nullptr) override;
    DeferenceableCoordinateSystem*
    resolve_ptr(token_iterator& tokens, DeferenceableCoordinateSystem* = nullptr) override;

  private:
    template<typename Key, typename Val>
    using UnorderedMultimap = Threads::SafeStructs::ThreadSafeUnorderedMultimap<Key, Val>;

    // Data protected by mutexes.
    UnorderedMultimap<uuid_type, SharedPtr<ExporterBase>> non_containers;
    UnorderedMultimap<uuid_type, SharedPtr<Container>>    containers;
    DeferenceableCoordinateSystem                         coordinate_system;

    /**
     * This is the pivot mutex. No one should use it directly.
     * @attention A SharedLock on it is not what you expect.
     * Use `mutex` instead.
     * @see MutexesWithPivot.
     */
    mutable Threads::GatherMutexData<Threads::MutexData,
                                     Threads::MutexData,
                                     Threads::MutexData>
        mutex{non_containers.getMutexLike(),
              containers.getMutexLike(),
              coordinate_system.getMutexLike()};

    mutable Threads::MutexSignal modified_sig{mutex};

  public:
    constexpr auto& getMutexLike() const { return mutex; }

    Threads::Signal<>& getChangedSignal() const override
    { return modified_sig; }

    Threads::Signal<SharedPtr<Container>>                  add_container_sig;
    Threads::Signal<SharedPtr<NamingScheme::ExporterBase>> add_non_container_sig;
    Threads::Signal<SharedPtr<Container>>                  remove_container_sig;
    Threads::Signal<SharedPtr<NamingScheme::ExporterBase>> remove_non_container_sig;
    Threads::Signal<SharedPtr<Container>, SharedPtr<Container>>
                                                           move_container_sig;
    Threads::Signal<SharedPtr<NamingScheme::ExporterBase>, SharedPtr<Container>>
                                                           move_non_container_sig;
  };

  static_assert(Threads::C_MutexHolder<Container>, "A container is a C_MutexHolder.");

}  // namespace Document
