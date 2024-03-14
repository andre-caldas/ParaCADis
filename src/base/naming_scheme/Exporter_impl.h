// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2023-2024 André Caldas <andre.em.caldas@gmail.com>       *
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

#ifndef NamingScheme_Exporter_impl_H
#define NamingScheme_Exporter_impl_H

#ifndef NamingScheme_Exporter_H
// The following error is because we do not want to incentivate developers
// to use "Exporter_impl.h" instead of "Exporter.h".
#  error "Include Exporter.h before including Exporter_impl.h. Do you need 'impl'?"
#endif
#include "Exporter.h"
#include "IExport.h"
#include "NameSearchResult.h"
#include "exceptions.h"
#include "types.h"

#include <base/expected_behaviour/SharedPtr.h>

#include <type_traits>

namespace NamingScheme
{

  template<typename X>
  ResolveData<X> Exporter::resolve(token_iterator& tokens)
  {
    ResolveData<X> resolve_data;
    bool result = resolve(tokens, &resolve_data);
    if(!) {
    }
    return resolve_data;
  }

  template<typename X>
  bool Exporter::_resolve(token_iterator& tokens, ResolveData<X>* resolve_data);
  {
    if (!tokens) { return; }

    // We set this before iterating, because we need the mutex for the last iteration.
    result->mutex = getMutexData();

    {  // Check if we directly export.
      // TODO: std::remove_cv<X>.
      auto ptr = dynamic_cast<IExport<X>*>(this);
      if (ptr) {
        auto next = ptr->resolve(tokens);
        if (next) {
          result->data = std::move(next);
          return result;
        }
      }
    }

    {  // Check if this is chainable.
      auto ptr = dynamic_cast<IExport<Exporter>*>(this);
      if (ptr) {
        auto pos           = tokens.cbegin();
        auto next_exporter = ptr->_resolve<X>(tokens, result);
        if (next_exporter) {
          assert(
              pos != tokens.cbegin()
              && "IExport<X>::resolve must advance iterator.");
          return next_exporter->resolve<X>(tokens, collector);
        }
      }
    }
  }
  throw ExceptionNoExport(tokens)
}

}  // namespace NamingScheme

#endif

