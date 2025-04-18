// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2023 André Caldas <andre.em.caldas@gmail.com>            *
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

/**
 * Template implementation for the interface defined by "IExport.h".
 * Users should not include this file if the exported type has its template
 * already instantiated at "IExport.cpp".
 */

#pragma once

#include "IExport.h"

#include "exceptions.h"
#include "Exporter.h"
#include "PathToken.h"

#include <base/expected_behaviour/SelfShared.h>
#include <base/threads/locks/MutexData.h>
#include <base/threads/locks/LockPolicy.h>
#include <base/threads/message_queue/Signal.h>

namespace NamingScheme
{
  template<typename T>
  ResultHolder<T> IExport<T>::resolve(
      const ResultHolder<ExporterCommon>& current,
      token_iterator& tokens, T*)
  {
    assert(Threads::LockPolicy::isLocked(current));
    assert(tokens && "Are you supposed to be calling this without any tokens?");
    if (!tokens) { return {}; }

    // Look for registered member variables.
    const auto& token = tokens.front();
    assert(token.isName());

    if constexpr (Threads::C_MutexHolder<T>) {
      auto shared = resolve_shared(tokens);
      if (shared) {
        return ResultHolder<T>{std::move(shared)};
      }
    } else {
      auto ptr = resolve_ptr(tokens);
      if (!ptr)
      {
        return {};
      }
      assert(!C_HasSharedPtr<T>
             && "Use resolve_shared() for types that inherit form SharedPtr");
      return {current, ptr};
    }
    return {};
  }


  /*
   * Recurrent templates to connect (assign proxy, actually) signals.
   */
  namespace {
    template<typename Sig>
    void connect_signals(SharedPtr<ExporterCommon>&& to, Sig& sig)
    { sig.setProxy(std::move(to), &ExporterCommon::child_changed_sig); }

    template<typename SigA, typename SigB, typename... Sigs>
    void connect_signals(SharedPtr<ExporterCommon>&& to, SigA& sig, SigB& sig2, Sigs&... sigs)
    {
        sig.setProxy(to, &ExporterCommon::child_changed_sig);
        connect_signals(std::move(to), sig2, sigs...);
    }
  }


  /*
   * Local pointer to type T.
   */
  template<typename T, class DataStruct, EachExportedData... dataInfo>
  requires TypeTraits::C_AllNamedDataOfType<T, T*, decltype(dataInfo)...>
  void IExportStruct<T, DataStruct, dataInfo...>::init()
  {
    static_assert(!std::is_base_of_v<ExporterCommon, T>, "ExporterCommon types need SharedPtrWrap.");
    static_assert(sizeof...(dataInfo) > 0, "Need at least an exported data.");

    if constexpr(C_HasChangedSignal<T>) {
      // Throws if not ExporterCommon sibling.
      // But this would be a bug: all IExport stuff must be a sibling of ExporterCommon.
      auto shared_ecommon = ExporterCommon::getSelfShared();

      assert(dynamic_cast<Exporter<DataStruct>*>(this)
             && "The exported structure must be provided by Exporter<...>.");

      auto& data = dynamic_cast<Exporter<DataStruct>&>(*this);
      auto& unprotected = data.safeData._unsafeStructAccess();
      connect_signals(std::move(shared_ecommon),
                      (unprotected.*(dataInfo.local_ptr)).getChangedSignal()...);
    }
  }

  template<typename T, class DataStruct, EachExportedData... dataInfo>
  requires TypeTraits::C_AllNamedDataOfType<T, T*, decltype(dataInfo)...>
  T* IExportStruct<T, DataStruct, dataInfo...>::resolve_ptr(token_iterator& tokens, T*)
  {
    assert(dynamic_cast<Exporter<DataStruct>*>(this)
           && "The exported structure must be provided by Exporter<...>.");

    if (!tokens) {
      assert(false && "Why is this being called? There are no more tokens!");
      return IExport<T>::resolve_ptr(tokens);
    }

    const auto& name = tokens.front().getName();
    if (!map.contains(name)) {
      return IExport<T>::resolve_ptr(tokens);
    }

    auto localPtr = map.at(name);
    tokens.advance(1);

    auto& data = dynamic_cast<Exporter<DataStruct>&>(*this);
    Threads::ReaderGate gate{data};
    return &(gate.getNonConst(data).*localPtr);
  }


  /*
   * Local pointer to type SharedPtrWrap<T>.
   */
  template<typename T, class DataStruct, EachExportedData... dataInfo>
  requires (TypeTraits::C_AllNamedDataOfType<T, SharedPtrWrap<T>, decltype(dataInfo)...>
            && Threads::C_MutexHolder<T>)
  void IExportStruct<T, DataStruct, dataInfo...>::init()
  {
    static_assert(sizeof...(dataInfo) > 0, "Need at least an exported data.");

    if constexpr(C_HasChangedSignal<T>) {
      auto shared_ecommon = ExporterCommon::getSelfShared();

      assert(dynamic_cast<Exporter<DataStruct>*>(this)
             && "The exported structure must be provided by Exporter<...>.");

      auto& data = dynamic_cast<Exporter<DataStruct>&>(*this);
      auto& unprotected = data.safeData._unsafeStructAccess();
      connect_signals(std::move(shared_ecommon),
                      (unprotected.*(dataInfo.local_ptr)).getSharedPtr()->getChangedSignal()...);
    }
  }

  template<typename T, class DataStruct, EachExportedData... dataInfo>
  requires (TypeTraits::C_AllNamedDataOfType<T, SharedPtrWrap<T>, decltype(dataInfo)...>
            && Threads::C_MutexHolder<T>)
  SharedPtr<T> IExportStruct<T, DataStruct, dataInfo...>::resolve_shared(token_iterator& tokens, T*)
  {
    assert(dynamic_cast<Exporter<DataStruct>*>(this)
           && "The exported structure must be provided by Exporter<...>.");

    if (!tokens) {
      assert(false && "Why is this being called? There are no more tokens!");
      return IExport<T>::resolve_shared(tokens);
    }

    const auto& name = tokens.front().getName();
    if (!map.contains(name)) {
      return IExport<T>::resolve_shared(tokens);
    }

    auto localPtr = map.at(name);
    tokens.advance(1);

    auto& data = dynamic_cast<Exporter<DataStruct>&>(*this);
    Threads::ReaderGate gate{data};
    return (gate.getNonConst(data).*localPtr).getSharedPtr();
  }
}
