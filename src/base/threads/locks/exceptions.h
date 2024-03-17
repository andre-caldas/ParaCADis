// SPDX-License-Identifier: LGPL-2.1-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2023 André Caldas <andre.em.caldas@gmail.com>            *
 *                                                                          *
 *   This file is part of FreeCAD.                                          *
 *                                                                          *
 *   FreeCAD is free software: you can redistribute it and/or modify it     *
 *   under the terms of the GNU Lesser General Public License as            *
 *   published by the Free Software Foundation, either version 2.1 of the   *
 *   License, or (at your option) any later version.                        *
 *                                                                          *
 *   FreeCAD is distributed in the hope that it will be useful, but         *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of             *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU       *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU Lesser General Public       *
 *   License along with FreeCAD. If not, see                                *
 *   <https://www.gnu.org/licenses/>.                                       *
 *                                                                          *
 ***************************************************************************/

#ifndef BASE_Threads_Exception_H
#define BASE_Threads_Exception_H

#include <base/exceptions.h>

namespace Threads
{

  class ExceptionNeedLock : public Base::RunTimeError
  {
  public:
    ExceptionNeedLock()
        : Base::RunTimeError("Cannot access unlocked data.")
    {
    }
  };

  class ExceptionNoExclusiveOverNonExclusive : public Base::RunTimeError
  {
  public:
    ExceptionNoExclusiveOverNonExclusive()
        : Base::RunTimeError(
              "Cannot lock exclusively a mutex that is already non-exclusive.")
    {
    }
  };

  class ExceptionExclusiveParentNotLocked : public Base::RunTimeError
  {
  public:
    ExceptionExclusiveParentNotLocked()
        : Base::RunTimeError(
              "An exclusive lock cannot come after non-chainable locks.")
    {
    }
  };

  class ExceptionNoLocksAfterExclusiveLock : public Base::RunTimeError
  {
  public:
    ExceptionNoLocksAfterExclusiveLock()
        : Base::RunTimeError(
              "After an exclusive lock there can be no other locks.")
    {
    }
  };

  class ExceptionNoLocksAfterLockFree : public Base::RunTimeError
  {
  public:
    ExceptionNoLocksAfterLockFree()
        : Base::RunTimeError(
              "You cannot lock anything while holding a 'lock free' lock.")
    {
    }
  };

  class ExceptionNeedLockToAccessContainer : public Base::RunTimeError
  {
  public:
    ExceptionNeedLockToAccessContainer()
        : Base::RunTimeError("You do not have a lock for the container you are "
                          "trying to access.")
    {
    }
  };

  class ExceptionCannotReleaseUnlocked : public Base::RunTimeError
  {
  public:
    ExceptionCannotReleaseUnlocked()
        : Base::RunTimeError("Cannot release lock that is not locked.")
    {
    }
  };

  class ExceptionNewThreadRequiresLock : public Base::RunTimeError
  {
  public:
    ExceptionNewThreadRequiresLock()
        : Base::RunTimeError(
              "To transfer a lock to a new thread, it has to be locked.")
    {
    }
  };

  class ExceptionNewThreadRequiresReleaseableLock : public Base::RunTimeError
  {
  public:
    ExceptionNewThreadRequiresReleaseableLock()
        : Base::RunTimeError(
              "Cannot move lock: thread remains locked even after release().")
    {
    }
  };

  class ExceptionNewThreadRequiresMovedLock : public Base::RunTimeError
  {
  public:
    ExceptionNewThreadRequiresMovedLock()
        : Base::RunTimeError(
              "To be transfered to a new thread, "
              "you need to call moveFromThread() in the original thread.")
    {
    }
  };

  class ExceptionNoNestedThreads : public Base::RunTimeError
  {
  public:
    ExceptionNoNestedThreads()
        : Base::RunTimeError(
              "Threads to process the same locked resource cannot be nested.")
    {
    }
  };

}  // namespace Threads

#endif  // BASE_Threads_Exception_H
