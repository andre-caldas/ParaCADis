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

#ifndef Threads_WriterLocks_impl_H
#define Threads_WriterLocks_impl_H

#include "writer_locks.h"

#include "MutexData.h"

#include <map>

namespace Threads {

  template<C_MutexLike... Mutex>
  ExclusiveLock::ExclusiveLock(Mutex&... mutexes)
      : LockPolicy(true, mutexes...)
  {
    lock();
  }


  template<C_MutexHolderWithGates ... Holders>
  WriterGate<Holders...>::WriterGate(Holders&... holders)
      : lock(getMutex(holders)...)
#ifndef NDEBUG
      , all_holders{&holders...}
#endif
  {
  }

  template<C_MutexHolderWithGates ... Holders>
  template<C_MutexHolderWithGates Holder>
  auto& WriterGate<Holders...>::operator[](Holder& holder) const
  {
    assert(all_holders.contains(&holder));
    return Holder::GateInfo::getData(holder);
  }


  template<C_MutexHolderWithGates Holder>
  WriterGate<Holder>::WriterGate(Holder& holder)
      : lock(getMutex(holder))
      , holder(holder)
  {
  }

  template<C_MutexHolderWithGates Holder>
  auto& WriterGate<Holder>::operator*()
  {
    return Holder::GateInfo::getData(holder);
  }

}

#endif
