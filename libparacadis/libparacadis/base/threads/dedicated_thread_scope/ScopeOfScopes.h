// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2025 André Caldas <andre.em.caldas@gmail.com>            *
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

#include "DedicatedThreadScope.h"

#include <libparacadis/base/expected_behaviour/SharedPtr.h>

namespace Threads
{
  class ScopeOfScopes;

  class ScopeOfScopesData
  {
    using strong_or_weak_ptr =
      std::pair<SharedPtr<DedicatedThreadScopeBase>,WeakPtr<DedicatedThreadScopeBase>>;
  public:
    ScopeOfScopesData() = default;

    void addScope(WeakPtr<DedicatedThreadScopeBase> scope);
    void addScopeKeepAlive(SharedPtr<DedicatedThreadScopeBase> scope);

  protected:
    friend class ScopeOfScopes;
    void execute() noexcept;

  private:
    std::vector<strong_or_weak_ptr> scopes;
  };

  /**
   * DedicatedThreadScope that keeps a victor of sub-scopes.
   *
   * @attention
   * There is no removeScope method.
   * If you use addScope(), a weak pointer is kept and the scope
   * is removed when the execution loop fails to lock the pointer.
   * If you use addScopeKeepAlive(), the sub-scope will never be removed.
   */
  class ScopeOfScopes
    : public DedicatedThreadScopeT<ScopeOfScopesData>
  {
  public:
    ScopeOfScopes() = default;

    /**
     * Adds a scope that will be automatically removed when
     * the weak pointer stops being valid.
     */
    void addScope(WeakPtr<DedicatedThreadScopeBase> scope);

    /**
     * Adds a scope that will never be removed.
     */
    void addScopeKeepAlive(SharedPtr<DedicatedThreadScopeBase> scope);
  };
}  // namespace Threads
