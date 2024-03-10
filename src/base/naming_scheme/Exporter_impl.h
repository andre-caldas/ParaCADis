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
#include "exceptions.h"
#include "types.h"

#include <base/expected_behaviour/SharedPtr.h>

namespace NamingScheme
{

  template<typename X>
  SharedPtr<X> Exporter::resolve(std::ranges::subrange<token_range> tokens);
  {
    {  // Check if this exports X.
      auto ptr = dynamic_cast<IExport<X>*>(this);
      if (ptr) { return ptr->resolve<X>(tokens); }
    }

    {  // Check if this is chainable.
      auto ptr = dynamic_cast<IExport<Exporter>*>(this);
      if (ptr) { return ptr->resolve<X>(tokens); }
    }
    throw ExceptionNoExport(tokens)
  }

}  // namespace NamingScheme

#endif  // NamingScheme_Exporter_impl_H

