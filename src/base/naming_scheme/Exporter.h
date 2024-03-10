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

#include <base/expected_behaviour/SharedPtr.h>
#include <base/expected_behaviour/SharedFromThis.h>
#include <base/types.h>

#include <ranges>
#include <string>

namespace NamingScheme
{

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
    virtual ~Exporter() = default;

    /**
     * @brief Calls the correct `resolve()` method.
     * The same class can export different types of objects.
     * That is, it can subclass different types of `IExport<T>`.
     * Each of them implements a different `resolve()` method.
     * This templated `resolve<>()` calls the correct one.
     */
    template<typename X>
    SharedPtr<X> resolve(std::ranges::subrange<token_range> tokens);

    /**
     * @brief Globally registers a Uuid.
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

    NameAndUuid name_and_uuid;
  };

}  // namespace NamingScheme

#endif  // NamingScheme_Exporter_H

