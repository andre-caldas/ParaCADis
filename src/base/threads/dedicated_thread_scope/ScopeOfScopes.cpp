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

#include "ScopeOfScopes.h"

namespace Threads
{
  void ScopeOfScopesData::addScope(WeakPtr<DedicatedThreadScopeBase> scope)
  {
    scopes.emplace_back(strong_or_weak_ptr{{}, std::move(scope)});
  }

  void ScopeOfScopesData::addScopeKeepAlive(SharedPtr<DedicatedThreadScopeBase> scope)
  {
    scopes.emplace_back(strong_or_weak_ptr{std::move(scope), {}});
  }

  void ScopeOfScopesData::execute() noexcept
  {
    auto it = scopes.begin();
    while(it != scopes.end()) {
      auto& [shared, weak] = *it;
      if(shared) {
        shared->execute();
        ++it;
        continue;
      }

      auto scope = weak.lock();
      if(scope) {
        scope->execute();
        ++it;
        continue;
      }
      it = scopes.erase(it);
    }
  }


  void ScopeOfScopes::addScope(WeakPtr<DedicatedThreadScopeBase> scope)
  {
    auto lambda = [scope=std::move(scope)](ScopeOfScopesData& data){
      data.addScope(std::move(scope));
      return false;
    };
    newAction(lambda);
  }

  void ScopeOfScopes::addScopeKeepAlive(SharedPtr<DedicatedThreadScopeBase> scope)
  {
    auto lambda = [scope=std::move(scope)](ScopeOfScopesData& data){
      data.addScopeKeepAlive(std::move(scope));
      return false;
    };
    newAction(lambda);
  }
}  // namespace Threads
