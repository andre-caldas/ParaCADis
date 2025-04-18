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

#pragma once

#include "Exporter.h"
#include "IExport.h"
#include "ResultHolder.h"

#include <concepts>

namespace Naming
{
  /**
   * Checks if a class can be chained. That is, if it exports anything.
   */
  template<class T>
  concept C_IsExporter = std::derived_from<T, ExporterCommon>;

  /**
   * An alias to make it easier to upcast a Chanable template.
   *
   * Otherwise, we would have an empty class descending from
   * IExport<ExporterCommon>.
   */
  using ChainableBase = IExport<ExporterCommon>;

  /**
   * Automatically looks for all exporters of descendendants of
   * ExporterCommon.
   *
   * Since we cannot sublcass IExport<IExport<T>> and
   * IExport<IExport<IExport<T>>> and so on for each possibility,
   * we blindly resolve for all exporters we export.
   *
   * @attention
   * Do not change <typename... EachChainable> to
   * <C_IsExporter... EachChainable> because the type EachChainable
   * maybe not defined yet (like a Container, that exportes itself).
   */
  template<typename... EachChainable>
  class Chainable
      : public ChainableBase
  {
  public:
    virtual ~Chainable() = default;

    /**
     * Virtual method that resolves an exporter.
     *
     * The final target is to resolve for a certain type `T`
     * (not of our knowledge). After trying to resolve `T` and not succeeding,
     * a Chainable will try to resolve for an Exporter in the "hope" that
     * this Exporter might resolve `T`.
     *
     * This actually calls `chain_resolve()` with all exported types
     * as template arguments.
     */
    ResultHolder<ExporterCommon> resolve(
        const ResultHolder<ExporterCommon>& current,
        token_iterator& tokens,
        ExporterCommon* = nullptr) override;

  private:
    /**
     * Tries to recursively resolve for all types First and Others.
     *
     * @attention
     * Do not change <typename... EachChainable> to
     * <C_IsExporter... EachChainable> because the type EachChainable
     * maybe not defined yet (like a Container, that exportes itself).
     * It will be very hard to debug!!! :-)
     */
    template<typename First, typename... Others>
    ResultHolder<ExporterCommon> chain_resolve(
        const ResultHolder<ExporterCommon>& current,
        token_iterator& tokens);
  };
}

#include "Chainable.hpp"
