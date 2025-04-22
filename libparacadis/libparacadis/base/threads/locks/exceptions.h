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

#ifndef BASE_Threads_Exception_H
#define BASE_Threads_Exception_H

#include <libparacadis/base/exceptions.h>

namespace Threads::Exception
{

  using namespace ::Exception;

  class NeedLock : public RunTimeError
  {
  public:
    NeedLock();
  };

  class NoExclusiveOverNonExclusive : public RunTimeError
  {
  public:
    NoExclusiveOverNonExclusive();
  };

  class ExclusiveParentNotLocked : public RunTimeError
  {
  public:
    ExclusiveParentNotLocked();
  };

  class NoLocksAfterExclusiveLock : public RunTimeError
  {
  public:
    NoLocksAfterExclusiveLock();
  };

  class NoLocksAfterLockFree : public RunTimeError
  {
  public:
    NoLocksAfterLockFree();
  };

  class NeedLockToAccessContainer : public RunTimeError
  {
  public:
    NeedLockToAccessContainer();
  };

  class CannotReleaseUnlocked : public RunTimeError
  {
  public:
    CannotReleaseUnlocked();
  };

  class NewThreadRequiresLock : public RunTimeError
  {
  public:
    NewThreadRequiresLock();
  };

  class NewThreadRequiresReleaseableLock : public RunTimeError
  {
  public:
    NewThreadRequiresReleaseableLock();
  };

  class NewThreadRequiresMovedLock : public RunTimeError
  {
  public:
    NewThreadRequiresMovedLock();
  };

  class NoNestedThreads : public RunTimeError
  {
  public:
    NoNestedThreads();
  };

  class AlreadyHasLayer : public RunTimeError
  {
  public:
    AlreadyHasLayer(int layer);
  };

  class AlreadyHasBiggerLayer : public RunTimeError
  {
  public:
    AlreadyHasBiggerLayer(int layer);
  };

}  // namespace Threads::Exception

#endif
