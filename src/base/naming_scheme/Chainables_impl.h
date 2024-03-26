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

#ifndef NamingScheme_Chainables_impl_H
#define NamingScheme_Chainables_impl_H

#include "Chainables.h"

using namespace NamingScheme;

template<typename... EachChainable>
SharedPtr<ExporterBase> Chainables<EachChainable...>::resolve_share(token_iterator &tokens, ExporterBase *)
{
  return chain_resolve<EachChainable...>(tokens);
}


template<typename... EachChainable>
template<C_IsChainable First, C_IsChainable... Others>
SharedPtr<ExporterBase> Chainables<EachChainable...>::chain_resolve(token_iterator& tokens)
{
  auto exporter = dynamic_cast<IExport<First>&>(*this);
  auto result = exporter.resolve(tokens);
  if(result)
  {
    return result;
  }

  if constexpr (sizeof...(Others) > 0)
  {
    return chain_resolve<Others...>(tokens);
  }
  return {};
}

#endif
