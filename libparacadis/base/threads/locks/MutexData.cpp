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

#include "MutexData.h"
#include "LockPolicy.h"

#include <base/threads/message_queue/MutexSignal.h>

namespace Threads
{

  void MutexData::report_exclusive_unlock() const
  {
    assert(LockPolicy::isLockedExclusively(*this)
           && "The signal must be sent while still holding an exclusive lock.");
    if(active_signals.empty()) { return; }
    for(auto signal: active_signals) {
      // Since we have an exclusive lock,
      // we assume the signal pointer is valid.
      signal->emit_signal();
    }
  }

}  // namespace Threads
