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

#pragma once

#include "reader_locks.h"

#include <libparacadis/base/expected_behaviour/SharedPtr.h>

#include <map>

namespace Threads {
  template<C_MutexLike... Mutex>
  SharedLock::SharedLock(Mutex&... mutex)
      : LockPolicy(false, mutex...)
  {
    lock();
  }

  template<C_MutexLike... Mutex>
  SharedLock::SharedLock(std::try_to_lock_t, Mutex&... mutex)
      : LockPolicy(false, mutex...)
  {
    try_lock();
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
  ReaderGate<Holders...>::ReaderGate(std::try_to_lock_t, const Holders&... holders)
      : lock{std::try_to_lock, getMutex(holders)...}
#ifndef NDEBUG
      , all_holders{&holders...}
#endif
  {
  }

  template<C_MutexHolderWithGates ... Holders>
  template<C_MutexHolderWithGates Holder>
  auto& ReaderGate<Holders...>::operator[](const Holder& holder) const
  {
    assert(!released && "Accessing data with released lock!");
    assert(all_holders.contains(&holder));
    return Holder::GateInfo::getData(holder);
  }

  template<C_MutexHolderWithGates ... Holders>
  void ReaderGate<Holders...>::release()
  {
    lock.release();
#ifndef NDEBUG
    released = true;
#endif
  }

  template<C_MutexHolderWithGates ... Holders>
  template<C_MutexHolderWithGates Holder>
  auto& ReaderGate<Holders...>::getNonConst(Holder& holder) const
  {
    assert(!released && "Accessing data with released lock!");
    assert(all_holders.contains(&holder));
    return Holder::GateInfo::getData(holder);
  }


  template<C_MutexHolderWithGates Holder>
  ReaderGate<Holder>::ReaderGate(const Holder& _holder)
      : lock(getMutex(_holder))
      , holder(_holder)
  {
  }

  template<C_MutexHolderWithGates Holder>
  ReaderGate<Holder>::ReaderGate(std::try_to_lock_t, const Holder& _holder)
      : lock(std::try_to_lock, getMutex(_holder))
      , holder(_holder)
  {
  }

  template<C_MutexHolderWithGates Holder>
  const auto& ReaderGate<Holder>::operator*() const
  {
    assert(!released && "Accessing data with released lock!");
    return Holder::GateInfo::getData(holder);
  }

  template<C_MutexHolderWithGates Holder>
  void ReaderGate<Holder>::release()
  {
    lock.release();
#ifndef NDEBUG
    released = true;
#endif
  }

  template<C_MutexHolderWithGates Holder>
  auto& ReaderGate<Holder>::getNonConst(Holder& h) const
  {
    assert(!released && "Accessing data with released lock!");
    assert(&h == &holder);
    return Holder::GateInfo::getData(h);
  }


  template<C_MutexHolderWithGates Holder>
  ReaderGateKeeper<Holder>::ReaderGateKeeper(Holder _holder)
      : lock(getMutex(_holder))
      , holder(std::move(_holder))
  {
  }

  template<C_MutexHolderWithGates Holder>
  const auto& ReaderGateKeeper<Holder>::operator*() const
  {
    if(released) {
      throw std::runtime_error{"Accessing a released gate."};
    }
    return Holder::GateInfo::getData(holder);
  }

  template<C_MutexHolderWithGates Holder>
  void ReaderGateKeeper<Holder>::release()
  {
    if(released) {
      throw std::runtime_error{"Gate released twice."};
    }
    released = true;
    lock.release();
  }
}
