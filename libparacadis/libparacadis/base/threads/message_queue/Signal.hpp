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

#include <libparacadis/base/threads/locks/reader_locks.h>
#include <libparacadis/base/threads/locks/writer_locks.h>

#include "SignalQueue.h"

#include <concepts>
#include <vector>

namespace Threads
{
  template<typename... Args>
  void Signal<Args...>::emit_signal(Args... args)
  {
    emit_signal_to_callbacks(args...);
    emit_signal_to_proxies(args...);
  }


  template<typename... Args>
  template<class SignalFrom, class SignalTo>
  int Signal<Args...>::connect(
      const SharedPtr<SignalFrom>& from,
      const SharedPtr<SignalQueue>& queue,
      const SharedPtr<SignalTo>& to,
      void (SignalTo::*member)(Args...))
  {
    auto lambda = [weak_from = from.getWeakPtr(),
                   weak_to = to.getWeakPtr(), member]
        (Args... args)
    {
      auto _from = weak_from.lock();
      if(!_from) {return;}
      auto _to = weak_to.lock();
      if(!_to){return;}
      ((*_to).*member)(args...);
    };

    WriterGate gate{callBacks};
    auto key = ++id;
    gate->emplace(key, Data{.queue_weak = queue.getWeakPtr(),
                            .to_lock_weak = to,
                            .call_back = std::move(lambda)});
    return key;
  }


  template<typename... Args>
  template<class SignalFrom, class SignalTo>
  int Signal<Args...>::connect(
      const SharedPtr<SignalFrom>& from,
      const SharedPtr<SignalQueue>& queue,
      const SharedPtr<SignalTo>& to,
      void (SignalTo::*member)(SharedPtr<SignalFrom>, Args...))
  {
    auto lambda = [weak_from = from.getWeakPtr(),
                   weak_to = to.getWeakPtr(), member]
        (Args... args)
    {
      auto _from = weak_from.lock();
      if(!_from) {return;}
      auto _to = weak_to.lock();
      if(!_to){return;}
      ((*_to).*member)(std::move(_from), args...);
    };

    WriterGate gate{callBacks};
    auto key = ++id;
    gate->emplace(key, Data{.queue_weak = queue.getWeakPtr(),
                            .to_lock_weak = to,
                            .call_back = std::move(lambda)});
    return key;
  }


  template<typename... Args>
  void Signal<Args...>::disconnect(int _id)
  {
    WriterGate gate{callBacks};
    gate->erase(_id);
  }


  template<typename... Args>
  void Signal<Args...>::emit_signal_to_proxies(Args... args)
  {
    // When the WeakPtr is no longer valid, we clean up.
    // To avoid a WriterGate, we save the callBack keys and delete later.
    std::vector<size_t> to_delete;

    std::vector<SharedPtr<signal_t>> locked_proxies;
    { // Scoped lock.
      ReaderGate gate{proxies};
      locked_proxies.reserve(gate->size());

      for(auto& [key, proxy_weak]: *gate)
      {
        auto proxy = proxy_weak.lock();
        if(proxy) {
          locked_proxies.emplace_back(std::move(proxy));
        } else {
          to_delete.push_back(key);
        }
      }
    }

    for(auto& proxy: locked_proxies) {
      proxy->emit_signal(args...);
    }

    // Clean up.
    if(!to_delete.empty()) {
      WriterGate gate{proxies};
      for(auto key: to_delete) {
        gate->erase(key);
      }
    }
  }


  template<typename... Args>
  void Signal<Args...>::emit_signal_to_callbacks(Args... args)
  {
    // When the WeakPtr is no longer valid, we clean up.
    // To avoid a WriterGate, we save the callBack keys and delete later.
    std::vector<int> to_delete;

    { // Scoped lock.
      ReaderGate gate{callBacks};
      for(auto& [key, data]: *gate)
      {
        // To avoid complexities,
        // we push_to_queue() while still holding the gate.
        // But of course we could reserve some vector of LockedData.
        // Things work fine because the signal queue uses MutexData::LOCKFREE locks.
        auto locked_data = data.lock();
        if(!locked_data.push_to_queue(args...)) {
          to_delete.push_back(key);
        }
      }
    }

    // Clean up.
    if(!to_delete.empty()) {
      WriterGate gate{callBacks};
      for(auto key: to_delete) {
        gate->erase(key);
      }
    }
  }


  template<typename... Args>
  bool Signal<Args...>::LockedData::push_to_queue(Args... args)
  {
#ifndef NDEBUG
    assert(!already_called && "Cannot use the same locked data twice.");
    already_called = true;
#endif

    if(!queue || !to_lock) {
      return false;
    }
    auto lambda = [cb = std::move(call_back), ...args = std::move(args)]{cb(args...);};
    queue->push(std::move(lambda), to_lock.getVoidPtr());
    return true;
  }


  template<typename... Args>
  template<typename Holder, typename SIG>
  size_t Signal<Args...>::setProxy(const SharedPtr<Holder>& holder, SIG Holder::* signal)
  {
    SharedPtr<signal_t> sig = holder.appendLocal(signal);
    size_t key = (size_t)sig.get();

    WriterGate gate{proxies};
    gate->try_emplace(key, sig);
    return key;
  }

  template<typename... Args>
  template<typename Holder, typename SIG>
  void Signal<Args...>::removeProxy(const SharedPtr<Holder>& holder, SIG Holder::* signal)
  {
    removeProxy(&(*holder).*signal);
  }

  template<typename... Args>
  void Signal<Args...>::removeProxy(size_t key)
  {
    WriterGate gate{proxies};
    gate->erase(key);
  }
}
