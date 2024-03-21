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

#include "exceptions.h"

#include <format>

namespace Threads::Exception
{
  NeedLock::NeedLock() : RunTimeError("Cannot access unlocked data.")
  {
  }

  NoExclusiveOverNonExclusive::NoExclusiveOverNonExclusive()
      : RunTimeError("Cannot lock exclusively a mutex that is already "
                     "non-exclusive.")
  {
  }

  ExclusiveParentNotLocked::ExclusiveParentNotLocked()
      : RunTimeError("An exclusive lock cannot come after non-chainable "
                     "locks.")
  {
  }

  NoLocksAfterExclusiveLock::NoLocksAfterExclusiveLock()
      : RunTimeError("After an exclusive lock there can be no other locks.")
  {
  }

  NoLocksAfterLockFree::NoLocksAfterLockFree()
      : RunTimeError("You cannot lock anything while holding a 'lock free' lock.")
  {
  }

  NeedLockToAccessContainer::NeedLockToAccessContainer()
      : RunTimeError("You do not have a lock for the container you are trying to access.")
  {
  }

  CannotReleaseUnlocked::CannotReleaseUnlocked()
      : RunTimeError("Cannot release lock that is not locked.")
  {
  }

  NewThreadRequiresLock::NewThreadRequiresLock()
      : RunTimeError("To transfer a lock to a new thread, it has to be locked.")
  {
  }

  NewThreadRequiresReleaseableLock::NewThreadRequiresReleaseableLock()
      : RunTimeError("Cannot move lock: thread remains locked even after release().")
  {
  }

  NewThreadRequiresMovedLock::NewThreadRequiresMovedLock()
      : RunTimeError("To be transfered to a new thread, "
                     "you need to call moveFromThread() in the original thread.")
  {
  }

  NoNestedThreads::NoNestedThreads()
      : RunTimeError("Threads to process the same locked resource cannot be nested.")
  {
  }

  AlreadyHasLayer::AlreadyHasLayer(int layer)
      : RunTimeError(std::format("Needs to create layer ({}), but it already exists", layer))
  {
  }

  AlreadyHasBiggerLayer::AlreadyHasBiggerLayer(int layer)
      : RunTimeError(
            std::format("Cannot use layer ({}) because it is not the top layer.", layer))
  {
  }

}  // namespace Threads::Exception
