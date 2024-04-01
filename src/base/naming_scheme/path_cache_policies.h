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

#ifndef NamingScheme_PathCachePolicies_H
#define NamingScheme_PathCachePolicies_H

#include "types.h"

#include <base/expected_behaviour/SharedPtr.h>

#include <chrono>

namespace NamingScheme
{

  class ExporterBase;

  class PathCachePolicyBase
  {
  public:
    /**
     * An opportunity to reserve cache space once you know the tokens.
     */
    virtual void prepare(token_iterator& tokens) = 0;
    virtual token_iterator topTokens() const = 0;
    virtual const SharedPtr<ExporterBase>& topExporter() const = 0;
    virtual void pushExporter(SharedPtr<ExporterBase>&& exporter, token_iterator tokens) = 0;
    virtual void invalidate() = 0;

    /**
     * Removes the last resolved ExporterBase.
     *
     * This may be used to:
     * 1. Discard part of the cache, according to policy.
     * 2. Discard the last ExporterBase, to treat it as the "final pointer".
     */
    virtual SharedPtr<ExporterBase> pop() = 0;

    /**
     * Discards part of the cache according to the policy.
     * @return True when the cache is still usable, meaning that
     * you may call NameSearchResult::resolveExporter().
     */
    virtual bool pruneCache() = 0;

    /**
     * The cache holds a chain of ExporterBase. It does not hold
     * the final pointer. This method indicates wheter the cache
     * determines whether the final pointer must be discarded or not.
     */
    virtual bool hasPartiallyExpired() const = 0;
  };


  class TimedWeakChain : public PathCachePolicyBase
  {
    using seconds_t    = std::chrono::seconds;
    using time_point_t = std::chrono::steady_clock::time_point;

  public:
    TimedWeakChain() = default;
    TimedWeakChain(seconds_t layer_duration) : layer_duration(layer_duration) {}

    void prepare(token_iterator& tokens) override;
    void invalidate() override;
    token_iterator topTokens() const override;
    const SharedPtr<ExporterBase>& topExporter() const override;
    void pushExporter(SharedPtr<ExporterBase>&& exporter, token_iterator tokens) override;

    bool pruneCache() override;
    SharedPtr<ExporterBase> pop() override;
    bool hasPartiallyExpired() const override;

  private:
    const seconds_t layer_duration{600};
    time_point_t    start;

    struct exporter_info_t
    {
      exporter_info_t(WeakPtr<ExporterBase> w, token_iterator t) noexcept
          : weak_ptr(std::move(w)), tokens(t) {}
      WeakPtr<ExporterBase> weak_ptr;
      token_iterator        tokens;
    };
    std::vector<exporter_info_t> exporters;
    SharedPtr<ExporterBase>      top_exporter;
  };

}  // namespace NamingScheme

#endif
