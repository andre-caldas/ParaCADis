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

#include "YesItIsAMutex.h"

#include <cassert>
#include <functional>

namespace Threads
{

#if 0
  void YesItIsAMutex::lock()
  {
    if (try_lock()) { return; }
    std::unique_lock lock{released_condition_lock};
    released.wait(lock, std::bind(&YesItIsAMutex::try_lock, this));
  }

  bool YesItIsAMutex::try_lock()
  {
    [[maybe_unused]] std::lock_guard lock{pivot};
    if (shared_counter > 0 || is_exclusively_locked) { return false; }
    is_exclusively_locked = true;
    return true;
  }

  void YesItIsAMutex::unlock()
  {
    {
      [[maybe_unused]] std::lock_guard lock{pivot};
      assert(is_exclusively_locked);
      is_exclusively_locked = false;
    }
    released.notify_one();
  }


  void YesItIsAMutex::lock_shared()
  {
    if (try_lock_shared()) { return; }
    std::unique_lock lock{released_condition_lock};
    released.wait(lock, std::bind(&YesItIsAMutex::try_lock_shared, this));
  }

  bool YesItIsAMutex::try_lock_shared()
  {
    [[maybe_unused]] std::lock_guard lock{pivot};
    if (is_exclusively_locked) { return false; }
    ++shared_counter;
    return true;
  }

  void YesItIsAMutex::unlock_shared()
  {
    [[maybe_unused]] std::lock_guard lock{pivot};
    assert(shared_counter > 0);
    --shared_counter;
    if (shared_counter <= 0) { released.notify_one(); }
  }
#endif

}  // namespace Threads
