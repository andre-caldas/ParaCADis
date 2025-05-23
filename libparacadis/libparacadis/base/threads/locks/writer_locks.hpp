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

#pragma once

#include "writer_locks.h"

#include "MutexData.h"

#include <map>

namespace Threads {
  template<C_MutexLike... Mutex>
  ExclusiveLock::ExclusiveLock(Mutex&... _mutexes)
      : LockPolicy(true, _mutexes...)
  {
    lock();
  }

  template<C_MutexLike... Mutex>
  ExclusiveLock::ExclusiveLock(std::try_to_lock_t, Mutex&... _mutexes)
      : LockPolicy(true, _mutexes...)
  {
    try_lock();
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
  WriterGate<Holders...>::WriterGate(std::try_to_lock_t, Holders&... holders)
      : lock{std::try_to_lock, getMutex(holders)...}
#ifndef NDEBUG
      , all_holders{&holders...}
#endif
  {
  }

  template<C_MutexHolderWithGates ... Holders>
  template<C_MutexHolderWithGates Holder>
  auto& WriterGate<Holders...>::operator[](Holder& holder) const
  {
    assert(!released && "Accessing data with released lock!");
    assert(all_holders.contains(&holder));
    return Holder::GateInfo::getData(holder);
  }

  template<C_MutexHolderWithGates ... Holders>
  void WriterGate<Holders...>::release()
  {
    lock.release();
    released = true;
  }


  template<C_MutexHolderWithGates Holder>
  WriterGate<Holder>::WriterGate(Holder& _holder)
      : lock(getMutex(_holder))
      , holder(_holder)
  {
  }

  template<C_MutexHolderWithGates Holder>
  WriterGate<Holder>::WriterGate(std::try_to_lock_t, Holder& _holder)
      : lock(std::try_to_lock, getMutex(_holder))
      , holder(_holder)
  {
  }

  template<C_MutexHolderWithGates Holder>
  auto& WriterGate<Holder>::operator*()
  {
    assert(!released && "Accessing data with released lock!");
    return Holder::GateInfo::getData(holder);
  }

  template<C_MutexHolderWithGates Holder>
  void WriterGate<Holder>::release()
  {
    lock.release();
    released = true;
  }


  template<C_MutexHolderWithGates Holder>
  WriterGateKeeper<Holder>::WriterGateKeeper(Holder _holder)
      : lock(getMutex(_holder))
      , holder(std::move(_holder))
  {
  }

  template<C_MutexHolderWithGates Holder>
  auto& WriterGateKeeper<Holder>::operator*() const
  {
    if(released) {
      throw std::runtime_error{"Accessing a released gate."};
    }
    return Holder::GateInfo::getData(holder);
  }

  template<C_MutexHolderWithGates Holder>
  void WriterGateKeeper<Holder>::release()
  {
    if(released) {
      throw std::runtime_error{"Gate released twice."};
    }
    released = true;
    lock.release();
  }
}
