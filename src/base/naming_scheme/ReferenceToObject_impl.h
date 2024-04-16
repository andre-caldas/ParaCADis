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

#ifndef NamingScheme_ReferenceToObject_impl_H
#define NamingScheme_ReferenceToObject_impl_H

#include "ReferenceToObject.h"

namespace NamingScheme {

template<typename T, std::derived_from<PathCachePolicyBase> CachePolicy>
ResultHolder<T> ReferenceTo<T, CachePolicy>::resolve() const
{
  auto result = searchResult.tryCache();
  if(result) {
    return result;
  }
  return searchResult.resolve(path.getRoot(), path.getTokens());
}

template<typename T, std::derived_from<PathCachePolicyBase> CachePolicy>
PathToObject& ReferenceTo<T, CachePolicy>::getPath()
{
  searchResult.invalidate();
  return path;
}

}

#endif
