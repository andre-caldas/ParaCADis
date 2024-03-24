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

#ifndef NamingScheme_NameSearchResult_impl_H
#define NamingScheme_NameSearchResult_impl_H

#include "Exporter.h"
#include "NameSearchResult.h"

#include <base/threads/locks/LockPolicy.h>

#include <type_traits>

using namespace NamingScheme;

template<typename T>
bool NameSearchResult<T>::resolve(token_iterator& tokens)
{
  // Resolve a chain of exporters.
  resolveExporter(tokens);

  // Do we want an exporter?
  if constexpr (std::is_same_v<std::remove_cv_t<T>, Exporter>) {
    if (tokens) {
      status = too_many_tokens;
      return false;
    }
    status = success;
    return true;
  }

  // If there are no more tokens.
  if (!tokens) {
    // Is the last exporter also an instance of T?
    auto ptr = dynamic_cast<T*>(exporter.get());
    if (ptr) {
      status = success;
      return true;
    }
    status = too_few_tokens;
    return false;
  }

  auto ptr = dynamic_cast<IExport<T>*>(exporter.get());
  if (!ptr) {
    status = does_not_export;
    return false;
  }

  data = ptr->resolve(tokens);
  if (!data) {
    status = not_found;
    return false;
  }

  if (tokens) {
    status = too_many_tokens;
    return false;
  }
  status = success;
  return true;
}


template<typename T>
SharedPtr<const T> NameSearchResult<T>::getDataForReading() const
{
  assert(Threads::LockPolicy::isLocked(exporter->getMutexData()));
  return data;
}

template<typename T>
SharedPtr<T> NameSearchResult<T>::getDataForWriting() const
{
  assert(Threads::LockPolicy::isLockedExclusively(exporter->getMutexData()));
  return data;
}

#endif
