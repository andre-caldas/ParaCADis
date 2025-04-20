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

#include "unreliable_mirror.h"
#include "reader_locks.h"
#include "writer_locks.h"

#include <cassert>

namespace Threads
{
  template<C_MutexHolderWithGates Holder>
  UnreliableMirrorGate<Holder>::UnreliableMirrorGate(SharedPtr<Holder> holder)
    : holder(std::move(holder))
  {}

  template<C_MutexHolderWithGates Holder>
  UnreliableMirrorGate<Holder>::~UnreliableMirrorGate()
  {
    { // shared lock scope
      [[maybe_unused]]
      SharedLock slock{*holder};
      try_update();
    }

    if(mirror != old_mirror)
    {
      [[maybe_unused]]
      ExclusiveLock elock{*holder};
      try_commit();
    }
  }

  template<C_MutexHolderWithGates Holder>
  auto& UnreliableMirrorGate<Holder>::operator*()
  {
    return mirror;
  }

  template<C_MutexHolderWithGates Holder>
  void UnreliableMirrorGate<Holder>::try_update()
  {
    ReaderGate gate{std::try_to_lock, *holder};
    if (gate) {
      mirror = *gate;
      gate.release();
      old_mirror = mirror;
    }
  }

  template<C_MutexHolderWithGates Holder>
  void UnreliableMirrorGate<Holder>::try_commit()
  {
    if (old_mirror == mirror) {
      return;
    }

    WriterGate gate{std::try_to_lock, *holder};
    if (gate) {
      *gate = mirror;
      gate.release();
      old_mirror = mirror;
    }
  }
}  // namespace Threads
