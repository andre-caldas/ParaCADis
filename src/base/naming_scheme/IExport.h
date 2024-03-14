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

#ifndef NamingScheme_IExport_H
#define NamingScheme_IExport_H

#include "types.h"

#include <base/expected_behaviour/SharedPtr.h>

#include <ranges>

namespace NamingScheme
{

  template <typename T>
  class NameSearchResult;

  /**
   * @brief Any class that exports some type T must subclass @class IExport<T>.
   *
   * @attention
   * In order for this class to be part of a chain of @class Exporter,
   * it does need to subclass @class Exporter.
   * We opted for **not making** `IExport<T>` a public virtual
   * subclass of @class Exporter and instead giving the developer the
   * **responsibility** to subclass it.
   * The optional template parameter may be used if you do not want
   * your class to derive from @class Exporter.
   */
  template<typename T>
  class IExport
  {
  public:
    using NameSearchResult = NameSearchResult<T>;
    using token_iterator = NamingScheme::token_iterator;

    SharedPtr<T> resolve(token_iterator& tokens);

  protected:
    /**
     * @brief Implement, to return a member of this object.
     * To be more precise, you can return a pointer if the pointer
     * is guaranteed to exist as long as this object exists.
     *
     * @attention To allow a class to derive from multiple IExport<T> classes,
     * we have added a T* to the end of the methods signature.
     */
    virtual T* resolve_ptr(token_iterator& tokens, T* = nullptr);

    /**
     * @brief Implement, to return a @class SharedPtr to any object.
     *
     * @attention To allow a class to derive from multiple IExport<T> classes,
     * we have added a T* to the end of the methods signature.
     */
    virtual SharefPtr<T> resolve_share(token_iterator& tokens, T* = nullptr);
  };

}  // namespace NamingScheme

#endif  // NamingScheme_IExport_H

