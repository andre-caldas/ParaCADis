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

#ifndef NamingScheme_NameSearch_impl_H
#define NamingScheme_NameSearch_impl_H

#include "Exporter.h"
#include "NameSearch.h"

#include <base/threads/locks/LockPolicy.h>
#include <base/threads/locks/reader_locks.h>

#include <type_traits>

using namespace NamingScheme;

template<typename T>
ResultHolder<T> NameSearch<T>::tryCache()
{
  if(!cache.hasPartiallyExpired()) {
    auto data = data_weak.getLockedShared();
    if(data) {
      return data;
    }
  }

  if(!cache.pruneCache()) {
    return {};
  }
  resolveExporter();
  return resolveLastStep();
}

template<typename T>
ResultHolder<T>
NameSearch<T>::resolve(SharedPtr<ExporterBase> root, token_iterator tokens)
{
  // TODO: assert using std::enable_shared_from_this that we do not need a mutex.
  cache.pushExporter(ResultHolder<ExporterBase>{std::move(root)}, tokens);
  resolveExporter();
  return resolveLastStep();
}

template<typename T>
void NameSearch<T>::resolveExporter()
{
  auto tokens = cache.topTokens();
  while (tokens) {
    auto exporter = cache.topExporter().cast<IExport<ExporterBase>>();
    if(!exporter) {
      return;
    }

    [[maybe_unused]]
    auto n_tokens = tokens.size();
    Threads::ReaderGate gate{exporter};
    auto next_exporter = gate.getNonConst(exporter)
                             .resolve(cache.topExporter(), tokens);
    if (!next_exporter) {
      return;
    }
    assert(n_tokens != tokens.size() && "Resolution is not consuming tokens.");
    assert(cache.topExporter() != next_exporter);
    cache.pushExporter(std::move(next_exporter), tokens);
  }
}

template<typename T>
ResultHolder<T> NameSearch<T>::resolveLastStep()
{
  auto tokens = cache.topTokens();

  // Are we looking for an exporter?
  if constexpr (std::is_same_v<std::remove_cv_t<T>, ExporterBase>) {
    if (tokens) {
      status = too_many_tokens;
      return {};
    }
    status = success;
    auto last = cache.pop();
    data_weak = last.getReleasedShared();
    return last;
  }

  // If there are no more tokens.
  if (!tokens) {
    auto last = cache.pop();
    auto ptr = last.cast<T>();
    if (ptr) {
      status = success;
      data_weak = ptr.getReleasedShared();
      return ptr;
    }
    status = too_few_tokens;
    return {};
  }

  auto exporter = cache.topExporter().cast<IExport<T>>();
  if (!exporter) {
    status = does_not_export;
    return {};
  }

  Threads::ReaderGate gate{exporter};
  auto data = gate.getNonConst(exporter)
                  .resolve(cache.topExporter(), tokens);
  if (!data) {
    status = not_found;
    return {};
  }

  if (tokens) {
    status = too_many_tokens;
    return {};
  }
  status = success;
  data_weak = data.getReleasedShared();
  return data;
}

#endif
