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
   * A Exporter is an object that can be queried to resolve the next step in a path.
   *
   * @attention For each exported type, you need to subclass @class IExport<X>.
   * @attention Probably you do not want to derive from this class.
   * You should probably derive from @class IExport<X> or @class Chainable.
   */
  class Exporter : public SharedFromThis<Exporter>
  {
  public:
    Exporter() = default;
    virtual ~Exporter() = default;

    /**
     * String for reports and diagnostics.
     */
    virtual std::string toString() const = 0;

    Uuid getUuid() const;
    std::string getName() const;

    /**
     * Must satisfy `Threads::C_MutexHolder`.
     */
    constexpr Threads::MutexData* getMutexData() { return &mutex; }

    /**
     * Globally registers a Uuid.
     *
     * This is specially useful when serializing
     * and unserializing.
     *
     * @param shared_ptr - a shared_ptr to the Exporter.
     */
    static void registerUuid(SharedPtr<Exporter> shared_ptr);

    /**
     * When serializing, uuids are saved as strings.
     * When unserializing, the string can be used
     * to get a pointer to Exporter.
     *
     * @param uuid - string representation of the uuid.
     * @return A shared_ptr to the referenced object.
     */
    static SharedPtr<Exporter> getByUuid(std::string uuid);

    /**
     * @brief Same as above.
     * @param uuid - the uuid.
     * @return A shared_ptr to the referenced object.
     */
    static SharedPtr<Exporter> getByUuid(Uuid::uuid_type uuid);

  private:
    Threads::MutexData mutex;
    NameAndUuid id;
  };

  static_assert(Threads::C_MutexHolder<Exporter>);


  template<typename DataStruct>
  class SafeExporter : public Exporter
  {
    using data_t = DataStruct;
    using safe_struct_t = Threads::SafeStructs::ThreadSafeStruct<data_t>;
  public:
    template<typename... Args>
    SafeExporter(Args&&... args) : safeData(*this, args...) {}

    safe_struct_t safeData;
  };

}  // namespace NamingScheme

#endif
