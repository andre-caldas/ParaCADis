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
    NameAndUuid name_and_uuid;

    Exporter() = default;
    Exporter(std::string name, std::vector<std::string> name_and_aliases = {});

    virtual ~Exporter() = default;

    /** Must satisfy `Threads::C_MutexHolder`.
     *
     * You may reimplement this if you have other mutexes in the same class.
     */
    virtual Threads::MutexData* getMutexData() { return &mutex_data; }

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

    const auto& getNameAndAliases() const { return name_and_aliases; }

  private:
    Threads::MutexData       mutex_data;
    std::vector<std::string> name_and_aliases;
  };

  static_assert(Threads::C_MutexHolder<Exporter>);

}  // namespace NamingScheme

#endif
