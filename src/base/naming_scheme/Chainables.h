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

#ifndef NamingScheme_Chainables_H
#define NamingScheme_Chainables_H

#include "Exporter.h"
#include "IExport.h"

#include <concepts>

namespace NamingScheme
{

  template<class T>
  concept C_IsChainable = std::derived_from<T, ExporterBase>;

  /**
   * Automatically looks for all exporters of descendendants of ExportBase.
   * @attention If you descend from Chainables, you cannot descend from
   * IExport<ExporterBase>.
   * However, you may reimplement resolve_share().
   * In your reimplementation, you probably want to first try
   * Chainables::resolve_share().
   * @attention We cannot use C_IsChainable, because we want to chain
   * with ourselves and std::derived_from cannot be used with incomplete types.
   */
  template<typename... EachChainable>
  class Chainables : public IExport<ExporterBase>
  {
  public:
    virtual ~Chainables() = default;

    ResultHolder<ExporterBase> resolve(const ResultHolder<ExporterBase>& current,
                                       token_iterator& tokens,
                                       ExporterBase* = nullptr) override;

  private:
    template<C_IsChainable First, C_IsChainable... Others>
    ResultHolder<ExporterBase> chain_resolve(const ResultHolder<ExporterBase>& current,
                                             token_iterator& tokens);
  };

}  // namespace NamingScheme

#include "Chainables_impl.h"

#endif
