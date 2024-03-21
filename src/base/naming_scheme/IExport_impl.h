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
 * @brief Template implementation for the interface defined by "IExport.h".
 * Users should not include this file if the exported type has its template
 * already instantiated at "IExport.cpp".
 */

#ifndef NamingScheme_IExport_impl_H
#define NamingScheme_IExport_impl_H

#ifdef NamingScheme_IExport_H
// The following error is because we do not want to incentivate developers
// to use "IExport_impl.h" instead of "IExport.h".
#  error "Include IExport.h before including IExport_impl.h. Do you need 'impl'?"
#endif
#include "IExport.h"  // Just to make tools happy!
#include "NameAndUuid.h"
#include "Exporter.h"

#include <base/expected_behaviour/SharedPtr.h>

namespace NamingScheme
{

  template<typename T>
  SharedPtr<T> IExport<T>::resolve(token_iterator& token_list)
  {
    if (!token_list) { return {}; }

    // Look for registered member variables.
    const auto& token = token_list.front();
    assert(token.isName());

    auto share = resolve_share(token_list);
    if (share) { return share; }

    auto ptr = resolve_ptr(token_list);
    if (ptr) { return std::shared_ptr(current_lock, ptr); }
    throw Exception::NoExport();
  }

  template<typename T>
  T* IExport<T>::resolve_ptr(token_iterator& /* token_list */)
  {
    return nullptr;
  }

  template<typename T>
  SharedPtr<T> IExport<T>::resolve_share(token_iterator& /* token_list */)
  {
    return {};
  }


  template<
      Threads::C_MutexHolderWithGates C, typename T,
      EachExportedData... dataInfo>
  const T* ExportedData<C, T, dataInfo...>::get(
      const C::ReaderGate& gate, std::string_view id) const
  {
    if (!map.contains(id)) {
      return nullptr;
    }
    return gate->*map.at(id);
  }

  template<
      Threads::C_MutexHolderWithGates C, typename T,
      EachExportedData<typename C::record_t, T>... dataInfo>
  T* ExportedData<C, T, dataInfo...>::get(
      const C::WriterGate& gate, std::string_view id) const
  {
    if (!map.contains(id)) {
      return nullptr;
    }

    return gate->*map.at(id);
  }


  template<typename T, typename DataStruct,
           EachExportedData<DataStruct, T>... dataInfo>
  T* SafeIExport<T, DataStruct, dataInfo...>::resolve_ptr(token_iterator& tokens)
  {
    if(!tokens) {
      assert(false && "Why is this being called? There are no more tokens!");
      return nullptr;
    }

    const auto& name = tokens.front().getName();
    if (!map.contains(name)) {
      return nullptr;
    }

    auto* ptr = dynamic_cast<SafeExporter<DataStruct>*>(this);
    assert(ptr && "Exporters must derive from NamingScheme::Exporter.");
    if(!ptr) {
      return nullptr;
    }

    auto& gate = ptr->getWriterGate();
    T* result = &gate->*map.at(name);
  }
    const std::map<std::string, T DataStruct::*> map
        = {{dataInfo.name,dataInfo.local_ptr}...};

}  // namespace NamingScheme

#endif
