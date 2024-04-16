// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2024 André Caldas <andre.em.caldas@gmail.com>            *
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

#ifndef Threads_ReaderLocks_impl_H
#define Threads_ReaderLocks_impl_H

#include "reader_locks.h"

#include <base/expected_behaviour/SharedPtr.h>

#include <map>

namespace Threads {

  template<C_MutexLike... Mutex>
  SharedLock::SharedLock(Mutex&... mutex)
      : LockPolicy(false, mutex...)
  {
    lock();
  }


  template<C_MutexHolderWithGates ... Holders>
  ReaderGate<Holders...>::ReaderGate(const Holders&... holders)
      : lock{getMutex(holders)...}
#ifndef NDEBUG
      , all_holders{&holders...}
#endif
  {
  }

  template<C_MutexHolderWithGates ... Holders>
  template<C_MutexHolderWithGates Holder>
  auto& ReaderGate<Holders...>::operator[](const Holder& holder) const
  {
    assert(all_holders.contains(&holder));
    return Holder::GateInfo::getData(holder);
  }

  template<C_MutexHolderWithGates ... Holders>
  template<C_MutexHolderWithGates Holder>
  auto& ReaderGate<Holders...>::getNonConst(Holder& holder) const
  {
    assert(all_holders.contains(&holder));
    return Holder::GateInfo::getData(holder);
  }


  template<C_MutexHolderWithGates Holder>
  ReaderGate<Holder>::ReaderGate(const Holder& holder)
      : lock(getMutex(holder))
      , holder(holder)
  {
  }

  template<C_MutexHolderWithGates Holder>
  const auto& ReaderGate<Holder>::operator*() const
  {
    return Holder::GateInfo::getData(holder);
  }

  template<C_MutexHolderWithGates Holder>
  auto& ReaderGate<Holder>::getNonConst(Holder& h) const
  {
    assert(&h == &holder);
    return Holder::GateInfo::getData(h);
  }


  template<C_MutexHolderWithGates Holder>
  ReaderGateKeeper<Holder>::ReaderGateKeeper(Holder&& holder)
      : lock(getMutex(holder))
      , holder(holder)
  {
  }

  template<C_MutexHolderWithGates Holder>
  const auto& ReaderGateKeeper<Holder>::operator*() const
  {
    return Holder::GateInfo::getData(holder);
  }

}

#endif
