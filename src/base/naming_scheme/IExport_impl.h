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
    throw ExceptionNoExport();
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

}  // namespace NamingScheme

#endif
