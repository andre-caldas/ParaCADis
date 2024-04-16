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

#include "IExport.h"  // Just to make tools happy!

#include "exceptions.h"
#include "Exporter.h"
#include "PathToken.h"

#include <base/threads/locks/MutexData.h>
#include <base/threads/locks/LockPolicy.h>

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


  template<typename T, class DataStruct, EachExportedData... dataInfo>
  T* IExportStruct<T, DataStruct, dataInfo...>::resolve_ptr(token_iterator& tokens, T*)
  {
    if (!tokens) {
      assert(false && "Why is this being called? There are no more tokens!");
      return nullptr;
    }

    const auto& name = tokens.front().getName();
    if (!map.contains(name)) {
      return nullptr;
    }
    auto localPtr = map.at(name);
    tokens.advance(1);

    Threads::WriterGate gate{data};
    return &((*gate).*localPtr);
  }

}  // namespace NamingScheme

#endif
