// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2023 André Caldas <andre.em.caldas@gmail.com>            *
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

#ifndef SafeStructs_ThreadSafeQueue_H
#define SafeStructs_ThreadSafeQueue_H

#include <base/threads/locks/gates.h>

#include <deque>

namespace Threads::SafeStructs
{

  /**
   * @brief Wraps an std::queue, into a mutex protected structure.
   */
  template<typename T>
  class ThreadSafeQueue
  {
  private:
    mutable MutexData mutex;
    std::deque<T>     container;

  public:
    using self_t = ThreadSafeQueue;
    using GateInfo = Threads::LocalGateInfo<&self_t::container,
                                            &self_t::mutex>;

    constexpr auto& getMutexLike() const { return mutex; }
  };

}

#endif
