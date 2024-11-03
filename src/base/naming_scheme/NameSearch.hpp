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
#include "NameSearch.h"

#include <base/threads/locks/LockPolicy.h>
#include <base/threads/locks/reader_locks.h>

#include <type_traits>

namespace NamingScheme
{
  template<typename T>
  ResultHolder<T> NameSearch<T>::tryCache()
  {
    auto result = cache.getFinalResult();
    if(result) {
      return result;
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
    assert(root == root->getSelfShared() && "Exporter cannot borrow a SharedPtr.");
    auto root_chainable = root.cast_nothrow<IExport<ExporterBase>>();
    if(!root_chainable) {
      cache.setExporter(ResultHolder{std::move(root)}, tokens);
    } else {
      cache.pushChainable(ResultHolder{std::move(root_chainable)}, tokens);
      resolveExporter();
    }
    return resolveLastStep();
  }

  template<typename T>
  void NameSearch<T>::resolveExporter()
  {
    auto tokens = cache.getTopTokens();
    auto next_chainable = cache.getTopChainable();
    while(next_chainable && tokens) {
      [[maybe_unused]] auto n_tokens = tokens.size();

      auto current = next_chainable.template cast<ExporterBase>();
      Threads::ReaderGate gate{next_chainable};
      auto next_exporter = gate.getNonConst(next_chainable)
                               .resolve(current, tokens);
      gate.release();

      if(!next_exporter) {
        // Fail.
        cache.setExporter(next_exporter, tokens);
        return;
      }
      next_chainable = next_exporter.template cast<IExport<ExporterBase>>();
      if(!next_chainable) {
        cache.setExporter(next_exporter, tokens);
        return;
      }

      assert(n_tokens != tokens.size() && "Resolution is not consuming tokens.");
      cache.pushChainable(std::move(next_chainable), tokens);
    }

    if(next_chainable) {
      auto next = next_chainable.template cast<ExporterBase>();
      cache.setExporter(next, tokens);
    }
  }

  template<typename T>
  ResultHolder<T> NameSearch<T>::resolveLastStep()
  {
    /*
     * Attention: before any return we need to have called
     * cache.invalidade() or cache.setFinalResult().
     */
    auto& exporter = cache.getLastExporter();
    if(!exporter) {
      cache.invalidate();
      return {};
    }

    auto tokens = cache.getTopTokens();

            // Are we looking for an exporter?
    if constexpr (std::is_same_v<std::remove_cv_t<T>, ExporterBase>) {
      if (tokens) {
        status = too_many_tokens;
        cache.invalidate();
        return {};
      }
      status = success;
      cache.setFinalResult(exporter);
      return exporter;
    }

    // If there are no more tokens.
    if (!tokens) {
      auto ptr = exporter.template cast<T>();
      if (ptr) {
        status = success;
        cache.setFinalResult(ptr);
        return ptr;
      }
      status = too_few_tokens;
      cache.invalidate();
      return {};
    }

    auto specific_exporter = exporter.template cast<IExport<T>>();
    if (!specific_exporter) {
      status = does_not_export;
      cache.invalidate();
      return {};
    }

    Threads::ReaderGate gate{specific_exporter};
    auto data = gate.getNonConst(specific_exporter)
                    .resolve(exporter, tokens);
    if (!data) {
      status = not_found;
      cache.invalidate();
      return {};
    }

    if (tokens) {
      status = too_many_tokens;
      cache.invalidate();
      return {};
    }

    status = success;
    cache.setFinalResult(data);
    return data;
  }
}
