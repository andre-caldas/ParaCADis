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

#ifndef NamingScheme_NameSearchResult_H
#define NamingScheme_NameSearchResult_H

#include "types.h"

#include <base/expected_behaviour/SharedPtr.h>
#include <base/threads/locks/reader_locks.h>
#include <base/threads/locks/writer_locks.h>

namespace NamingScheme
{

  class ExporterBase;

  /**
   * All information for the resolved ReferenceToOjbect.
   *
   * To support multithread, NamingScheme::Exporter provides a shared_mutex
   * for accessing its data. So, we keep a reference for this mutex.
   * The developer must lock the mutex before using the data.
   */
  class NameSearchResultBase
  {
  protected:
    SharedPtr<ExporterBase> exporter;

    void resolveExporter(token_iterator& tokens);

  public:
    NameSearchResultBase(SharedPtr<ExporterBase> root) : exporter(std::move(root)) {}
  };


  template<typename T>
  class NameSearchResult : public NameSearchResultBase
  {
  private:
    SharedPtr<T> data;

  public:
    using NameSearchResultBase::NameSearchResultBase;

    bool resolve(token_iterator& tokens);

    enum {
      not_resolved_yet = '0',  ///< Method resolve() not called, yet.
      success          = 'S',  ///< Method resolve() succeeded.
      not_found        = '?',  ///< Could not find token.
      too_few_tokens   = '<',  ///< Tokens resolve only up to Exporter.
      too_many_tokens  = '>',  ///< Resolved, but did not consume all tokens.
      does_not_export  = '*'   ///< Last Exporter does not export requested type.
    } status = not_resolved_yet;

    SharedPtr<const T> getDataForReading() const;
    SharedPtr<T>       getDataForWriting() const;
  };

}  // namespace NamingScheme

#include "NameSearchResult_impl.h"

#endif
