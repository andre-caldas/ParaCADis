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

#pragma once

#include "NameAndUuid.h"
#include "types.h"

#include <base/expected_behaviour/SelfShared.h>
#include <base/expected_behaviour/SharedPtr.h>
#include <base/threads/locks/LockPolicy.h>
#include <base/threads/locks/MutexData.h>
#include <base/threads/message_queue/MutexSignal.h>
#include <base/threads/safe_structs/ThreadSafeStruct.h>

#include <concepts>
#include <string>
#include <utility>

namespace NamingScheme
{
  template<typename T>
  class IExport;

  template<typename T>
  concept C_HasChangedSignal = requires(T a) {
    a.getChangedSignal();
    {a.getChangedSignal()} -> std::same_as<Threads::Signal<>&>;
  };

  /**
   * A ExporterBase is an object that can be queried to resolve the next step in a path.
   *
   * @attention For each exported type, you need to subclass IExport<X>.
   * @attention Probably you do not want to derive from this class.
   * You should probably derive from IExport<X> or Chainable.
   */
  class ExporterBase : public SelfShared<ExporterBase>
  {
  public:
    // In IExport<>, generates ambiguity, so we put it here.
    using token_iterator = NamingScheme::token_iterator;

  protected:
    ExporterBase()               = default;
    ExporterBase(ExporterBase&&) = default;

    /// Do not really copy anything, because copies
    /// must have a different id and probably should have
    /// a different name.
    ExporterBase(const ExporterBase&) = delete;

  public:
    virtual ~ExporterBase() = default;

    /**
     * To be signaled when an exported child object changes.
     * Child modified_sig are chained to this signal by the
     * IExport<> constructor.
     */
    mutable Threads::Signal<> child_changed_sig;

    /**
     * Descendants of ExporterBase must implement a deepCopyExporter() method.
     *
     * @attention The policy is:
     * 1. Implement a deepCopy() specific for the derived type.
     * 2. Implement a deepCopyExporter() to return the result of deepCopy().
     */
    virtual SharedPtr<ExporterBase> deepCopyExporter() const = 0;

    /**
     * The signal that indicates some change has happened.
     *
     * Every change shall be done with an ExclusiveLock.
     * The returned signal must be associated with every mutex.
     * In practice, what is really signaled is the release of the exclusive lock.
     */
    virtual Threads::Signal<>& getChangedSignal() const = 0;

    /**
     * String for reports and diagnostics.
     *
     * TODO: make pure virtual.
     */
    virtual std::string toString() const { return "Implement ExporterBase::toString()!"; }

    Uuid        getUuid() const;
    std::string getName() const;
    void        setName(std::string name);

    operator Uuid() const;
    operator Uuid::uuid_type() const;

    /**
     * Globally registers a Uuid.
     *
     * This is specially useful when serializing
     * and unserializing.
     *
     * @param shared_ptr - a shared_ptr to the ExporterBase.
     */
    static void registerUuid(SharedPtr<ExporterBase> shared_ptr);

    /**
     * Search registered object by uuid.
     *
     * @param uuid - string representation of the uuid.
     * @return A shared_ptr to the referenced object.
     */
    static SharedPtr<ExporterBase> getByUuid(std::string uuid);

    /**
     * Search registered object by uuid.
     *
     * @param uuid - the uuid.
     * @return A shared_ptr to the referenced object.
     */
    static SharedPtr<ExporterBase> getByUuid(Uuid::uuid_type uuid);

  private:
    NameAndUuid id;
  };


  template<typename DataStruct>
  class Exporter
      : public virtual ExporterBase
  {
  public:
    using data_t        = DataStruct;
    using safe_struct_t = Threads::SafeStructs::ThreadSafeStruct<data_t>;

    safe_struct_t safeData;

    Threads::Signal<>& getChangedSignal() const override
    { return modified_sig; }

    /**
     * Must satisfy `Threads::C_MutexHolder`.
     */
    constexpr auto& getMutexLike() const { return safeData.getMutexLike(); }

    /**
     * Signaled when exclusive (write) mutex is released.
     */
    mutable Threads::MutexSignal modified_sig{getMutexLike()};

  protected:
    Exporter() = default;

    /**
     * We disable copy and moving because of the signals to parent
     * that we would have to disconnect.
     */
    /// @{
    Exporter(Exporter&&) = delete;
    Exporter(const Exporter&) = delete;
    Exporter& operator=(Exporter&&) = delete;
    Exporter& operator=(const Exporter&) = delete;
    /// @}

    template<typename... T>
    Exporter(T&&... t)
        : safeData(std::forward<T>(t)...)
    {}

  public:
    using GateInfo = Threads::LocalBridgeInfo<&Exporter::safeData>;
  };

  namespace detail {
    struct test {};
  }

  static_assert(
      Threads::C_MutexHolder<Exporter<detail::test>>,
      "Exporter should implement C_MutexHolder.");

  static_assert(
      C_HasChangedSignal<ExporterBase>,
      "ExporterBase must signalize changes.");
}  // namespace NamingScheme
