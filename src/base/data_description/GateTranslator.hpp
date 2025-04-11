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

#include "GateTranslator.h"

#include <base/threads/locks/reader_locks.h>
#include <base/threads/locks/writer_locks.h>

#include <cassert>

namespace DataDescription
{
  template<Threads::C_MutexHolderWithGates Inner, typename... User>
  GateTranslator<Inner, User...>::GateTranslator(SharedPtr<Inner> _inner)
      : inner(std::move(_inner))
      , cache(*Threads::ReaderGate{inner})
      , user(cache->user)
  {
    inner->getChangedSignal().connect(
        inner, signal_queue, cache, &cache_t::slotInnerChanged);
  }

  template<Threads::C_MutexHolderWithGates Inner, typename... User>
  GateTranslator<Inner, User...>
      ::GateTranslator(SharedPtr<Inner> _inner, user_t& user_cache)
    requires C_SimpleSubTranslator<cache_t>
      : inner(std::move(_inner))
      , cache(*Threads::ReaderGate{*inner}, user_cache)
      , user(cache->user)
  {
  }

  template<Threads::C_MutexHolderWithGates Inner, typename... User>
  void GateTranslator<Inner, User...>::innerToUser()
  {
    if(!cache->hasInnerChanged()) {
      return;
    }
    Threads::ReaderGate gate{*inner};
    cache->update(*gate, user);
  }

  template<Threads::C_MutexHolderWithGates Inner, typename... User>
  bool GateTranslator<Inner, User...>::tryInnerToUser()
  {
    if(!cache->hasInnerChanged()) {
      return true;
    }

    Threads::ReaderGate gate{std::try_to_lock, *inner};
    if(!gate) {
      return false;
    }
    cache->update(*gate, user);
    return true;
  }

  template<Threads::C_MutexHolderWithGates Inner, typename... User>
  void GateTranslator<Inner, User...>::userToInner()
  {
    if(user == cache->user) {
      return;
    }
    Threads::WriterGate gate{*inner};
    cache->commit(*gate, user);
  }

  template<Threads::C_MutexHolderWithGates Inner, typename... User>
  bool GateTranslator<Inner, User...>::tryUserToInner()
  {
    if(user == cache->user) {
      return true;
    }
    Threads::WriterGate gate{std::try_to_lock, *inner};
    if(!gate) {
      return false;
    }
    cache->commit(*gate, user);
    return true;
  }
}
