// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2024 André Caldas <andre.em.caldas@gmail.com>            *
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

#include "ResultHolder.h"
#include "types.h"

#include <chrono>

namespace NamingScheme
{
  class ExporterBase;
  template<typename T>
  class IExport;

  template<typename T>
  class PathCachePolicyBase
  {
  public:
    /**
     * An opportunity to reserve cache space once you know the tokens.
     */
    virtual void prepare(token_iterator& tokens) = 0;

    /**
     * Tokens that were not processed yet.
     */
    virtual token_iterator getTopTokens() const = 0;
    virtual ResultHolder<IExport<ExporterBase>> getTopChainable() const = 0;
    virtual const ResultHolder<ExporterBase>& getLastExporter() const = 0;

    /**
     * The final result, if not expired.
     */
    virtual ResultHolder<T> getFinalResult() const = 0;

    virtual void setFinalResult(ResultHolder<T> result) = 0;
    virtual void setExporter(ResultHolder<ExporterBase> exporter, token_iterator tokens) = 0;
    virtual void pushChainable(
        ResultHolder<IExport<ExporterBase>> exporter, token_iterator tokens) = 0;
    virtual void invalidate() = 0;

    /**
     * Discards part of the cache according to the policy.
     *
     * @returns
     * True when the cache is still usable, meaning that
     * you may call NameSearch::resolveExporter().
     */
    virtual bool pruneCache() = 0;
  };


  template<typename T>
  class TimedWeakChain : public PathCachePolicyBase<T>
  {
    using seconds_t    = std::chrono::seconds;
    using time_point_t = std::chrono::steady_clock::time_point;

  public:
    TimedWeakChain() = default;
    TimedWeakChain(seconds_t layer_duration) : layer_duration(layer_duration) {}

    void prepare(token_iterator& tokens) override;
    void invalidate() override;

    token_iterator getTopTokens() const override;
    ResultHolder<IExport<ExporterBase>> getTopChainable() const override;
    const ResultHolder<ExporterBase>& getLastExporter() const override;
    ResultHolder<T> getFinalResult() const override;

    void setFinalResult(ResultHolder<T> final) override;
    void setExporter(ResultHolder<ExporterBase> exporter, token_iterator tokens) override;
    void pushChainable(
        ResultHolder<IExport<ExporterBase>> exporter, token_iterator tokens) override;

    bool pruneCache() override;
    bool hasPartiallyExpired() const;

  private:
    const seconds_t layer_duration{600};
    time_point_t    start;

    template<typename X>
    struct exporter_info_t
    {
      exporter_info_t() = default;
      exporter_info_t(ResultHolder<X> h, token_iterator t) noexcept
          : tokens(t), holder(std::move(h))
      {}

      token_iterator  tokens;
      ResultHolder<X> holder;

      void reset() { holder = {}; tokens = {}; }
      operator bool() const { return holder; }
    };

    std::vector<exporter_info_t<IExport<ExporterBase>>> chainables;
    exporter_info_t<ExporterBase>                       last_exporter;
    ResultHolder<T>                                     final_result;
  };
}

#include "path_cache_policies.hpp"
