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
#include "ResultHolder.h"
#include "types.h"

#include <base/threads/locks/reader_locks.h>
#include <base/threads/locks/writer_locks.h>

#include <concepts>

namespace NamingScheme
{
  class ExporterCommon;
  template<typename T>
  class PathCachePolicyBase;

  /**
   * All information for the resolved ReferenceToOjbect.
   */
  template<typename T>
  class NameSearch
  {
  public:
    NameSearch(PathCachePolicyBase<T>& _cache) : cache(_cache) {}

    /**
     * Resolves without trying the cache.
     *
     * @attention
     * This does not automatically call tryCache()
     * because we don't want anyone calling resolve without first
     * trying the cache. If the cache succeds, you do not need to
     * acquire a @a root.
     *
     * @attention
     * We take an ExporterCommon instead of ExporterCommon
     * because this method is intended to be called "externally".
     * That is, we expect it to actually be called by passing
     * SharedPtr<SomeExporterType>, where SomeExporterType
     * descends from ExporterCommon.
     * But we actually require SomeExporterType to descend from
     * ExporterCommon. A downcast will be done internally.
     */
    ResultHolder<T> resolve(SharedPtr<ExporterCommon> root, token_iterator tokens);

    /**
     * Tries the cache.
     */
    ResultHolder<T> tryCache();

    /**
     * Invalidates the cache.
     */
    void invalidateCache() { cache.invalidate(); }

  protected:
    void            resolveExporter(void);
    ResultHolder<T> resolveLastStep(void);

    enum {
      not_resolved_yet = '0',  ///< Method resolve() not called, yet.
      success          = 'S',  ///< Method resolve() succeeded.
      not_found        = '?',  ///< Could not find token.
      too_few_tokens   = '<',  ///< Tokens resolve only up to Exporter.
      too_many_tokens  = '>',  ///< Resolved, but did not consume all tokens.
      does_not_export  = '*'   ///< Last Exporter does not export requested type.
    } status = not_resolved_yet;

  private:
    /// Cache for the search.
    PathCachePolicyBase<T>& cache;
  };
}

#include "NameSearch.hpp"
