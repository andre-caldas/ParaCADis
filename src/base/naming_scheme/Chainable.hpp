// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2023-2025 André Caldas <andre.em.caldas@gmail.com>       *
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

#pragma once

#include "Chainable.h"

using namespace NamingScheme;

template<typename... EachChainable>
ResultHolder<ExporterCommon>
Chainable<EachChainable...>::resolve(const ResultHolder<ExporterCommon>& current,
                                      token_iterator& tokens, ExporterCommon*)
{
  auto chain_result = Chainable<EachChainable...>::chain_resolve<EachChainable...>(current, tokens);
  if(chain_result) {
    return chain_result;
  }
  return IExport<ExporterCommon>::resolve(current, tokens);
}


template<typename... EachChainable>
template<typename First, typename... Others>
ResultHolder<ExporterCommon>
Chainable<EachChainable...>::chain_resolve(
    const ResultHolder<ExporterCommon>& current, token_iterator& tokens)
{
  auto& exporter = dynamic_cast<IExport<First>&>(*this);
  auto result = exporter.resolve(current, tokens);
  if(result)
  {
    return result.template cast<ExporterCommon>();
  }

  if constexpr(sizeof...(Others) > 0)
  {
    return chain_resolve<Others...>(current, tokens);
  }

  return {};
}
