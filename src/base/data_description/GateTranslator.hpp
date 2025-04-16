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
  template<Threads::C_MutexHolderWithGates Inner, typename User>
  GateTranslator<Inner, User>::GateTranslator(SharedPtr<Inner> _inner)
      : inner(std::move(_inner))
      , cache(*Threads::ReaderGate{*inner})
      , user(cache->user)
  {
    inner->getChangedSignal().connect(
        inner, signal_queue, change_sentinel, &ChangeSentinel::slotInnerChanged);
  }

  template<Threads::C_MutexHolderWithGates Inner, typename User>
  GateTranslator<Inner, User>
      ::GateTranslator(SharedPtr<Inner> _inner, user_t& user_cache)
    requires C_StructSubTranslator<cache_t>
      : inner(std::move(_inner))
      , cache(*Threads::ReaderGate{*inner}, user_cache)
      , user(cache->user)
  {
    inner->getChangedSignal().connect(
        inner, signal_queue, change_sentinel, &ChangeSentinel::slotInnerChanged);
  }


  template<Threads::C_MutexHolderWithGates Inner, typename User>
  void GateTranslator<Inner, User>::innerToUser()
  {
    for(auto* sub: cache->getSubTranslators()) {
      sub->innerToUser();
    }

    signal_queue->try_run();
    if(!change_sentinel->hasInnerChanged()) {
      return;
    }

    change_sentinel->resetInnerChanged();
    Threads::ReaderGate gate{*inner};
    cache->update(*gate, user);
  }


  template<Threads::C_MutexHolderWithGates Inner, typename User>
  void GateTranslator<Inner, User>::tryInnerToUser()
  {
    for(auto* sub: cache->getSubTranslators()) {
      sub->tryInnerToUser();
    }

    signal_queue->try_run();
    if(!change_sentinel->hasInnerChanged()) {
      return;
    }

    Threads::ReaderGate gate{std::try_to_lock, *inner};
    if(!gate) {
      return;
    }
    change_sentinel->resetInnerChanged();
    cache->update(*gate, user);
    return;
  }


  template<Threads::C_MutexHolderWithGates Inner, typename User>
  void GateTranslator<Inner, User>::userToInner()
  {
    // We assume the same user and same cache->user for all sub translators.
    if(user == cache->user) {
      return;
    }

    for(auto* sub: cache->getSubTranslators()) {
      sub->userToInner();
    }

    Threads::WriterGate gate{*inner};
    cache->commit(*gate, user);
  }


  template<Threads::C_MutexHolderWithGates Inner, typename User>
  void GateTranslator<Inner, User>::tryUserToInner()
  {
    // We assume the same user and same cache->user for all sub translators.
    if(user == cache->user) {
      return;
    }

    for(auto* sub: cache->getSubTranslators()) {
      sub->tryUserToInner();
    }

    Threads::WriterGate gate{std::try_to_lock, *inner};
    if(!gate) {
      return;
    }
    cache->commit(*gate, user);
    return;
  }
}
