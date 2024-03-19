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
#include <vector>

namespace NamingScheme
{

  template<typename X>
  class NameSearchResult;

  template<typename T>
  class IExport;

  /**
   * @brief A Exporter is an object that can be queried
   * to resolve the next step in a path.
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
     * Must satisfy `Threads::C_MutexHolder`.
     */
    virtual Threads::MutexData* getMutexData() = 0;

    /** Globally registers a Uuid.
     * This is specially useful when serializing (Save)
     * and unserializing (Restore).
     * @param shared_ptr - a shared_ptr to the @class Exporter.
     */
    static void registerUuid(const SharedPtr<Exporter>& shared_ptr);

    /**
     * @brief When serializing (Save), uuids are saved as strings.
     * When unserializing (Restore), the string can be used
     * to get a pointer to @class Exporter.
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
  };

  static_assert(Threads::C_MutexHolder<Exporter>);


  template<typename DataStruct>
  class SafeExporter : public Exporter
  {
    using data_t = DataStruct;
    using safe_struct_t = Threads::SafeStructs::ThreadSafeStruct<data_t>;

  public:
    safe_struct_t safeData;
  };

}  // namespace NamingScheme

#endif
