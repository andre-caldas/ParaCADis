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

#ifndef NamingScheme_NameSearch_H
#define NamingScheme_NameSearch_H

#include "path_cache_policies.h"
#include "ResultHolder.h"
#include "types.h"

#include <base/threads/locks/reader_locks.h>
#include <base/threads/locks/writer_locks.h>

#include <concepts>

namespace NamingScheme
{

  class ExporterBase;
  class PathCachePolicyBase;

  /**
   * All information for the resolved ReferenceToOjbect.
   */
  template<typename T>
  class NameSearch
  {
  public:
    NameSearch(PathCachePolicyBase& cache) : cache(cache) {}

    /**
     * Resolves without trying the cache.
     * @attention This does not automatically call tryCache()
     * because we don't want anyone calling resolve without first
     * trying the cache. If the cache succeds, you do not need to
     * acquire a @a root.
     */
    ResultHolder<T> resolve(SharedPtr<ExporterBase> root, token_iterator tokens);

    /**
     * Tries the cache.
     */
    ResultHolder<T> tryCache();

    /**
     * Invalidates the cache.
     */
    void invalidateCache() { data_weak = {}; cache.invalidate(); }

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
    /// Cache for the "chainable" part of the path.
    PathCachePolicyBase&  cache;
    /// Cache for the path final node (that might not be chainable).
    ResultHolder<T>       data_weak;
  };

}  // namespace NamingScheme

#include "NameSearch_impl.h"

#endif
