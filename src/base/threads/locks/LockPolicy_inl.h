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

#ifndef Threads_LockPolicy_inc_H
#define Threads_LockPolicy_inc_H

#include "LockPolicy.h"
#include "exceptions.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <mutex>
#include <shared_mutex>

namespace Threads
{

  template<C_MutexData... MutN>
  LockPolicy::LockPolicy(bool is_exclusive, MutN&... mutex) : mutexes{&mutex...}
  {
    _processLock(is_exclusive);
  }

  template<typename... MutN>
  LockPolicy::LockPolicy(bool is_exclusive, MutN&... mutex)
  {
    unfold_mutexes(mutex...);
    _processLock(is_exclusive);
  }

  template<C_GatherMutexData G, typename... MutN>
  void LockPolicy::unfold_mutexes(G& g, MutN&... mutex)
  {
    unfold_mutexes(g.first, g.others, mutex...);
  }

  template<C_MutexData M, typename... MutN>
  void LockPolicy::unfold_mutexes(M& m, MutN&... mutex)
  {
    unfold_mutexes(mutex..., m);
  }

  template<C_MutexData ... MutN>
  void LockPolicy::unfold_mutexes(MutN&... mutex)
  {
    mutexes = {&mutex...};
  }

}  // namespace Threads

#endif
