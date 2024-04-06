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

#ifndef NamingScheme_IExport_H
// The following error is because we do not want to incentivate developers
// to use "IExport_impl.h" instead of "IExport.h".
#  error "Include IExport.h before including IExport_impl.h. Do you need 'impl'?"
#endif
#include "Exporter.h"
#include "IExport.h"  // Just to make tools happy!
#include "PathToken.h"
#include "exceptions.h"

#include <base/expected_behaviour/SharedPtr.h>

namespace NamingScheme
{

  template<typename T>
  SharedPtr<T> IExport<T>::resolve(const SharedPtr<ExporterBase>& current,
                                   token_iterator& tokens, T*)
  {
    if (!tokens) { return {}; }

    // Look for registered member variables.
    const auto& token = tokens.front();
    assert(token.isName());

    auto share = resolve_share(tokens);
    if (share) { return share; }

    auto ptr = resolve_ptr(tokens);
    if (ptr) { return std::shared_ptr<T>(current.sliced(), ptr); }

    return {};
  }

  template<typename T>
  T* IExport<T>::resolve_ptr(token_iterator& /* token_list */, T*)
  {
    return nullptr;
  }

  template<typename T>
  SharedPtr<T> IExport<T>::resolve_share(token_iterator& /* token_list */, T*)
  {
    return {};
  }


  template<Threads::C_MutexHolderWithGates C, typename T, EachExportedData... dataInfo>
  const T*
  ExportedData<C, T, dataInfo...>::get(const C::ReaderGate& gate, std::string_view id) const
  {
    if (!map.contains(id)) { return nullptr; }
    return gate->*map.at(id);
  }

  template<Threads::C_MutexHolderWithGates C, typename T, EachExportedData... dataInfo>
  T* ExportedData<C, T, dataInfo...>::get(const C::WriterGate& gate, std::string_view id) const
  {
    if (!map.contains(id)) { return nullptr; }
    return gate->*map.at(id);
  }


  template<typename T, class DataStruct, EachExportedData... dataInfo>
  T* SafeIExport<T, DataStruct, dataInfo...>::resolve_ptr(token_iterator& tokens, T*)
  {
    if (!tokens) {
      assert(false && "Why is this being called? There are no more tokens!");
      return nullptr;
    }

    auto* ptr = dynamic_cast<Exporter<DataStruct>*>(this);
    assert(ptr && "Exporters must derive from NamingScheme::Exporter.");
    if (!ptr) { return nullptr; }

    auto        gate = ptr->getWriterGate();
    const auto& name = tokens.front().getName();
    if (!map.contains(name)) { return nullptr; }

    return &((*gate).*map.at(name));
  }

}  // namespace NamingScheme

#endif
