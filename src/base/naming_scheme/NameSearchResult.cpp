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

#include "Exporter.h"

#include <base/threads/locks/LockPolicy.h>

#include <memory>

using namespace Threads;

namespace NamingScheme
{

  void NameSearchResultBase::resolveExporter(token_iterator& tokens)
  {
    while (tokens) {
      auto ptr = dynamic_cast<IExport<Exporter>*>(exporter.get());
      if (!ptr) {
        return;
      }
      [[maybe_unused]]
      auto n_tokens = tokens.size();
      auto next_exporter = ptr->resolve<Exporter>(tokens);
      if (!next_exporter) {
        return;
      }
      assert(n_tokens != tokens.size() && "Resolution is not consuming tokens.");
      assert(next_exporter != exporter);
      exporter = std::move(next_exporter);
    }
  }

  SharedLock NameSearchResultBase::lockForReading() const
  {
    return SharedLock(exporter->getMutexData());
  }

  ExclusiveLock<MutexData> NameSearchResultBase::lockForWriting() const
  {
    return ExclusiveLock(exporter->getMutexData());
  }

}  // namespace NamingScheme

/**
 * Template instantiation for most used exported types.
 */

#include <base/geometric_primitives/Circle.h>
#include <base/geometric_primitives/Line.h>
#include <base/geometric_primitives/types.h>

template class NamingScheme::NameSearchResult<Real>;
template class NamingScheme::NameSearchResult<Point>;
template class NamingScheme::NameSearchResult<Vector>;
template class NamingScheme::NameSearchResult<Line>;
template class NamingScheme::NameSearchResult<Circle>;

