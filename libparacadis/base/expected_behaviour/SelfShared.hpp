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

#include "SelfShared.h"
#include "SharedPtr.h"

#include <concepts>

template<typename T>
SharedPtr<T> SelfShared<T>::getSelfShared()
{
  return this->shared_from_this();
}

template<typename T>
WeakPtr<T> SelfShared<T>::getSelfWeak()
{
#ifndef NDEBUG
  return getSelfShared();
#else
  return this->weak_from_this();
#endif
}

template<typename T>
template<typename X>
SharedPtr<X> SelfShared<T>::getSelfShared()
{
  if constexpr(std::same_as<X,T>) {
    return getSelfShared();
  } else {
    auto shared = this->shared_from_this();
    assert(shared && "This object is not managed by a SharedPtr.");
    auto casted = std::dynamic_pointer_cast<X>(shared);
    assert(casted && "Could not dynamic cast. Maybe not a related type.");
    return casted;
  }
}

template<typename T>
template<typename X, typename C>
SharedPtr<X> SelfShared<T>::getLocalShared(X C::* local)
{
  auto shared = this->shared_from_this();
  assert(shared && "This object is not managed by a SharedPtr.");
  auto casted = std::dynamic_pointer_cast<C>(shared);
  assert(casted && "Could not dynamic cast. Maybe not a related type.");
  return std::shared_ptr<X>{std::move(casted), (*casted).*local};
}
