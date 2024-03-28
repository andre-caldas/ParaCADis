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

#ifndef NamingScheme_Exporter_H
#define NamingScheme_Exporter_H

#include "NameAndUuid.h"
#include "types.h"

#include <base/expected_behaviour/SharedFromThis.h>
#include <base/expected_behaviour/SharedPtr.h>
#include <base/threads/locks/LockPolicy.h>
#include <base/threads/locks/MutexData.h>
#include <base/threads/safe_structs/ThreadSafeStruct.h>

#include <string>

namespace NamingScheme
{

  template<typename X>
  class NameSearchResult;

  template<typename T>
  class IExport;

  /**
   * A ExporterBase is an object that can be queried to resolve the next step in a path.
   *
   * @attention For each exported type, you need to subclass IExport<X>.
   * @attention Probably you do not want to derive from this class.
   * You should probably derive from IExport<X> or Chainable.
   */
  class ExporterBase : public SharedFromThis<ExporterBase>
  {
  public:
    // In IExport<> generates ambiguity, so we put it here.
    using token_iterator = NamingScheme::token_iterator;

    ExporterBase(const ExporterBase&) = delete;

    ExporterBase()          = default;
    virtual ~ExporterBase() = default;

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
     * Must satisfy `Threads::C_MutexHolder`.
     */
     /// @{
    constexpr Threads::MutexData& getMutexData() const { return mutex; }
    constexpr operator Threads::MutexData&() const { return mutex; }
     /// @}

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
     * When serializing, uuids are saved as strings.
     * When unserializing, the string can be used
     * to get a pointer to ExporterBase.
     *
     * @param uuid - string representation of the uuid.
     * @return A shared_ptr to the referenced object.
     */
    static SharedPtr<ExporterBase> getByUuid(std::string uuid);

    /**
     * @brief Same as above.
     * @param uuid - the uuid.
     * @return A shared_ptr to the referenced object.
     */
    static SharedPtr<ExporterBase> getByUuid(Uuid::uuid_type uuid);

  protected:
    mutable Threads::MutexData mutex;

  private:
    NameAndUuid id;
  };

  static_assert(
      Threads::C_MutexHolder<ExporterBase>,
      "ExporterBase should implement C_MutexHolder.");


  template<typename DataStruct>
  class Exporter : public ExporterBase
  {
    using data_t        = DataStruct;
    using safe_struct_t = Threads::SafeStructs::ThreadSafeStruct<data_t>;

  public:
    Exporter(const Exporter&) = delete;
    template<typename... Args>
    Exporter(Args&&... args) : safeData(*this, args...)
    {
    }

    auto getReaderGate() const noexcept { return safeData.getReaderGate(); }
    auto getWriterGate() noexcept { return safeData.getWriterGate(); }

  private:
    safe_struct_t safeData;
  };

}  // namespace NamingScheme

#endif
