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

#include "path_cache_policies.h"

#include "Exporter.h"

#include <base/expected_behaviour/SharedPtr.h>

#include <cmath>
#include <ranges>

using namespace NamingScheme;

void TimedWeakChain::prepare(token_iterator& tokens)
{
  top_exporter.reset();
  exporters.clear();
  exporters.reserve(tokens.size());
  start = std::chrono::steady_clock::now();
}

void TimedWeakChain::invalidate()
{
  top_exporter.reset();
  exporters.clear();
}

token_iterator TimedWeakChain::topTokens() const
{
  assert(!exporters.empty());
  return exporters.back().tokens;
}

const SharedPtr<ExporterBase>& TimedWeakChain::topExporter() const
{
  return top_exporter;
}

void TimedWeakChain::pushExporter(SharedPtr<ExporterBase>&& exporter, token_iterator tokens)
{
  exporters.emplace_back(exporter, tokens);
  top_exporter = std::move(exporter);
}

bool TimedWeakChain::pruneCache()
{
  if(exporters.size() == 0) {
    return false; // Cache not useful.
  }

  // First, we declare the cache as hasPartiallyExpired().
  // After that we start popping from `exporters`. Therefore, the `-1`.
  std::size_t prune_count = (std::chrono::steady_clock::now() - start) / layer_duration - 1;
  if(prune_count > 0) {
    prune_count = std::max(prune_count, exporters.size());
    exporters.erase(exporters.end()-prune_count, exporters.end());
  }

  while(!exporters.empty()) {
    auto& info = exporters.back();
    if(auto ptr = info.weak_ptr.lock()) {
      top_exporter = ptr;
      return true;
    }
  }
  return false;
}

SharedPtr<ExporterBase> TimedWeakChain::pop()
{
  assert(!exporters.empty());
  auto result = exporters.back().weak_ptr.lock();
  exporters.pop_back();
  return result;
}

bool TimedWeakChain::hasPartiallyExpired() const
{
  return (std::chrono::steady_clock::now() - start) > layer_duration;
}
