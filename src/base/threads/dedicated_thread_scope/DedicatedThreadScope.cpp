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

#include "DedicatedThreadScope.h"

#include <exception>
#include <iostream>

namespace Threads
{
  void DedicatedThreadScopeBase::execute() noexcept
  {
    auto it = callables.begin();
    while(it != callables.end()) {
      bool keep = true;
      try {
        keep = (*it)();
      } catch(const std::exception& e) {
        std::cerr << "Exception caught in rednering queue: " << e.what() << ".\n";
      } catch(...) {
        std::cerr << "Unkown exception caught in rendering queue.\n";
      }
      if(!keep) {
        it = callables.erase(it);
      } else {
        ++it;
      }
    }
  }

  void DedicatedThreadScopeBase::appendCallable(inner_callable_t callable)
  {
    callables.emplace_back(std::move(callable));
  }
}  // namespace Threads
