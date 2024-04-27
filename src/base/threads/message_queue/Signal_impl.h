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

#ifndef MessageQueue_Signal_impl_H
#define MessageQueue_Signal_impl_H

#include <base/threads/locks/writer_locks.h>

#include "SignalQueue.h"

#include <concepts>

namespace Threads
{

  template<typename... Args>
  void Signal<Args...>::emit_signal(Args... args)
  {
    WriterGate gate{callBacks};
    for(auto it = callBacks.begin(); it != callBacks.end();)
    {
      auto qlock = it->second.queue_weak.lock();
      auto tolock = it->second.to_void_weak.lock();
      if(qlock && tolock) {
        qlock->push([cb = it->second.call_back, args...]{cb(args...);});
        ++it;
      } else {
        it = gate->erase(it);
      }
    }
  }


  template<typename... Args>
  template<class T, std::derived_from<T> SignalFrom, class SignalTo>
  int Signal<Args...>::connect(
      const SharedPtr<SignalFrom>& from,
      const SharedPtr<SignalQueue>& queue,
      const SharedPtr<SignalTo>& to,
      void (SignalTo::*member)(SharedPtr<T>, Args...))
  {
    auto lambda = [weak_from = from.getWeakPtr(),
                   weak_to = to.getWeakPtr(), member]
        (Args... args)
    {
      auto from = weak_from.lock();
      if(!from) {return;}
      auto to = weak_to.lock();
      if(to){to->*member(from, args...);}
    };

    WriterGate gate{callBacks};
    auto key = ++id;
    gate->emplace({key, {queue.getWeakPtr(), to.getWeakPtr(), std::move(lambda)}});
    return key;
  }


  template<typename... Args>
  void Signal<Args...>::disconnect(int id)
  {
    WriterGate gate{callBacks};
    gate->erase(id);
  }

}

#endif
