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

#ifndef NamingScheme_ReferenceToObject_H
#define NamingScheme_ReferenceToObject_H

#include "NameSearchResult.h"
#include "PathToObject.h"
#include "path_cache_policies.h"
#include "types.h"

#include <concepts>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace NamingScheme
{

  /**
   * A  PathToObject is not aware of the type of variable it points to.
   *
   * Through the means of the templated  IExport<variable_type>,
   * objects can "export" the correct type to be referenced to by
   * ReferenceTo<variable_type>.
   *
   * @example
   * ReferenceTo<double, TimedWeakChain> ref(root, "start point", "x");
   *
   * @see  IExport<>.
   */
  template<typename T, std::derived_from<PathCachePolicyBase> CachePolicy>
  class ReferenceTo
  {
  public:
    ReferenceTo(ReferenceTo&&)                 = default;
    ReferenceTo(const ReferenceTo&)            = default;
    ReferenceTo& operator=(const ReferenceTo&) = default;
    ReferenceTo& operator=(ReferenceTo&&)      = default;

    template<typename... Args>
    ReferenceTo(Args&&... args) : path(std::forward<Args>(args)...) {}

    /**
     * Fully resolves the chain up to the last token.
     */
    SharedPtr<T> resolve() const;

    /**
     * Gets a reference to the PathToObject.
     * @attention This call invalidates the cache.
     */
    PathToObject& getPath();

  private:
    PathToObject        path;
    CachePolicy         cache;
    NameSearchResult<T> searchResult{cache};
  };

}  // namespace NamingScheme

#endif
