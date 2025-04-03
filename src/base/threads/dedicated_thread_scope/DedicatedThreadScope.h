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

#include "../safe_structs/ThreadSafeQueue.h"

#include <functional>
#include <list>

namespace Threads
{
  class DedicatedThreadScopeBase
  {
  public:
    virtual ~DedicatedThreadScopeBase() = default;
    virtual void execute();

  protected:
    using inner_callable_t = std::function<bool()>;
    using callable_list_t = std::list<inner_callable_t>;
    using callable_iter_t = callable_list_t::const_iterator;

    void appendCallable(inner_callable_t callable);

  private:
    callable_list_t callables;
  };

  template<typename ProtectedStruct>
  class DedicatedThreadScopeT
    : protected DedicatedThreadScopeBase
  {
  public:
    using struct_t = ProtectedStruct;
    using callable_t = std::function<bool(struct_t&)>;

    void execute() override;
    void newAction(callable_t callable);

  private:
    struct_t theStruct;
    SafeStructs::ThreadSafeQueue<callable_t> queue;
  };
}  // namespace Threads

#include "DedicatedThreadScope.hpp"
