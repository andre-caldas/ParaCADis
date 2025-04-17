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

#include "path_cache_policies.h"

#include "Exporter.h"

#include <cmath>
#include <ranges>

// TODO: indent! :-(
namespace NamingScheme
{

template<typename T>
void TimedWeakChain<T>::prepare(token_iterator& tokens)
{
  assert(chainables.empty()
         && "Cannot call prepare() on an already prepared cache.");
  assert(!last_exporter
         && "Cannot call prepare() on an already prepared cache.");
  assert(!final_result
         && "Cannot call prepare() on an already prepared cache.");
  chainables.reserve(tokens.size());
  start = std::chrono::steady_clock::now();
}

template<typename T>
void TimedWeakChain<T>::invalidate()
{
  chainables.clear();
  last_exporter.reset();
  final_result = {};
}

template<typename T>
token_iterator TimedWeakChain<T>::getTopTokens() const
{
  assert(!final_result
         && "The getTopTokens() information is valid only during search.");

  if(last_exporter) {
    return last_exporter.tokens;
  }
  assert(!chainables.empty());
  return chainables.back().tokens;
}

template<typename T>
TimedWeakChain<T>::chainable_holder_t
TimedWeakChain<T>::getTopChainable() const
{
  if(chainables.empty()) {
    return {};
  }
  auto last_chainable = chainables.back();
  assert(last_chainable
         && "Last chainable is supposed to be locked.");
  return last_chainable.holder;
}

template<typename T>
const TimedWeakChain<T>::exporter_holder_t&
TimedWeakChain<T>::getLastExporter() const
{
  assert((chainables.empty() || !chainables.back())
         && "All chainables must be unlocked.");
  return last_exporter.holder;
}

template<typename T>
ResultHolder<T> TimedWeakChain<T>::getFinalResult() const
{
  if(hasPartiallyExpired()) {
    return {};
  }

  auto result = final_result.getLockedShared();
  return result;
}

template<typename T>
void TimedWeakChain<T>::setFinalResult(ResultHolder<T> final)
{
  final_result = std::move(final);
  final_result.releaseShared();
  assert(!final_result);
  last_exporter.holder.releaseShared();
}

template<typename T>
void TimedWeakChain<T>::setExporter(
    exporter_holder_t exporter, token_iterator tokens)
{
  assert(exporter && "Cannot set an invalid exporter.");
  last_exporter = {std::move(exporter), tokens};
  if(!chainables.empty()) {
    chainables.back().holder.releaseShared();
  }
}

template<typename T>
void TimedWeakChain<T>::pushChainable(
    chainable_holder_t chainable, token_iterator tokens)
{
  assert(!last_exporter && "Cannot set chainable after non-chainable.");
  assert(chainable && "Chainable is supposed to be locked.");
  if(!chainables.empty()) {
    chainables.back().holder.releaseShared();
  }
  chainables.emplace_back(std::move(chainable), tokens);
}

template<typename T>
bool TimedWeakChain<T>::pruneCache()
{
  assert(!last_exporter && "Last resolution is supposed to have finished.");
  last_exporter.holder.lockShared();

  // First, we declare the cache as hasPartiallyExpired().
  std::size_t prune_count = (std::chrono::steady_clock::now() - start) / layer_duration;
  if(prune_count == 0 && last_exporter) {
    assert(final_result.getLockedShared()
           && "We expect final_result is valid as long as last_exporter is.");
    // Notice that last_exporter is locked.
    return true;
  }
  --prune_count;

  last_exporter = {};
  final_result = {};
  if(!chainables.empty()) {
    auto erase_count = std::max(prune_count, chainables.size());
    chainables.erase(chainables.end() - erase_count, chainables.end());
  }

  while(!chainables.empty()) {
    auto& info = chainables.back();
    info.holder.lockShared();
    if(info.holder) {
      // Notice that last chainable is locked.
      assert(chainables.back() && "Last element is supposed to be locked.");
      return true;
    }
    chainables.pop_back();
  }
  assert(chainables.empty());
  return false;
}

template<typename T>
bool TimedWeakChain<T>::hasPartiallyExpired() const
{
  return (std::chrono::steady_clock::now() - start) > layer_duration;
}

}
