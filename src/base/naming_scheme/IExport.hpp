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

#include <base/threads/locks/MutexData.h>
#include <base/threads/locks/LockPolicy.h>
#include <base/threads/message_queue/Signal.h>

namespace NamingScheme
{
  template<typename T>
  ResultHolder<T> IExport<T>::resolve(const ResultHolder<ExporterBase>& current,
                                      token_iterator& tokens, T*)
  {
    assert(Threads::LockPolicy::isLocked(current));
    assert(tokens && "Are you supposed to be calling this without any tokens?");
    if (!tokens) { return {}; }

    // Look for registered member variables.
    const auto& token = tokens.front();
    assert(token.isName());

    auto shared = resolve_shared(tokens);
    if (shared) { return ResultHolder<T>{shared}; }

    auto ptr = resolve_ptr(tokens);
    if (ptr)
    {
      return {current, ptr};
    }

    return {};
  }

  template<typename T>
  T* IExport<T>::resolve_ptr(token_iterator& /* token_list */, T*)
  {
    return nullptr;
  }

  template<typename T>
  SharedPtr<T> IExport<T>::resolve_shared(token_iterator& /* token_list */, T*)
  {
    return {};
  }


  /*
   * Recurrent templates to connect (assign proxy, actually) signals.
   */
  namespace {
    template<typename Sig>
    void connect_signals(SharedPtr<ExporterBase>&& to, Sig& sig)
    { sig.setProxy(std::move(to), &ExporterBase::child_changed_sig); }

    template<typename SigA, typename SigB, typename... Sigs>
    void connect_signals(SharedPtr<ExporterBase>&& to, SigA& sig, SigB& sig2, Sigs&... sigs)
    {
        sig.setProxy(to, &ExporterBase::child_changed_sig);
        connect_signals(std::move(to), sig2, sigs...);
    }
  }


  /*
   * Local pointer to type T.
   */
  template<typename T, class DataStruct, EachExportedData... dataInfo>
  requires C_AllExportedDataOfType<T, T, decltype(dataInfo)...>
  void IExportStruct<T, DataStruct, dataInfo...>::init()
  {
    static_assert(!std::is_base_of_v<ExporterBase, T>, "ExporterBase types need SharedPtrWrap.");
    static_assert(sizeof...(dataInfo) > 0, "Need at least an exported data.");

    if constexpr(C_HasChangedSignal<T>) {
      ExporterBase& ebase = dynamic_cast<ExporterBase&>(*this);
      auto shared_ebase = ebase.getSelfShared();

      assert(dynamic_cast<Exporter<DataStruct>*>(this)
             && "The exported structure must be provided by Exporter<...>.");

      auto& data = dynamic_cast<Exporter<DataStruct>&>(*this);
      auto& unprotected = data.safeData._unsafeStructAccess();
      connect_signals(std::move(shared_ebase),
                      (unprotected.*(dataInfo.local_ptr)).getChangedSignal()...);
    }
  }

  template<typename T, class DataStruct, EachExportedData... dataInfo>
  requires C_AllExportedDataOfType<T, T, decltype(dataInfo)...>
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
  requires C_AllExportedDataOfType<T, SharedPtrWrap<T>, decltype(dataInfo)...>
  void IExportStruct<T, DataStruct, dataInfo...>::init()
  {
    static_assert(sizeof...(dataInfo) > 0, "Need at least an exported data.");

    if constexpr(C_HasChangedSignal<T>) {
      ExporterBase& ebase = dynamic_cast<ExporterBase&>(*this);
      auto shared_ebase = ebase.getSelfShared();

      assert(dynamic_cast<Exporter<DataStruct>*>(this)
             && "The exported structure must be provided by Exporter<...>.");

      auto& data = dynamic_cast<Exporter<DataStruct>&>(*this);
      auto& unprotected = data.safeData._unsafeStructAccess();
      connect_signals(std::move(shared_ebase),
                      (unprotected.*(dataInfo.local_ptr)).getSharedPtr()->getChangedSignal()...);
    }
  }

  template<typename T, class DataStruct, EachExportedData... dataInfo>
  requires C_AllExportedDataOfType<T, SharedPtrWrap<T>, decltype(dataInfo)...>
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
