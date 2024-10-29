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

#include "SharedPtr.h"  // Just to make tools happy!

#include <cassert>
#include <memory>
#include <optional>
#include <type_traits>

template<typename T>
SharedPtr<T> SharedPtr<T>::from_pointer(T* ptr)
{
  return std::shared_ptr<T>(ptr);
}

template<typename T>
SharedPtr<T>::SharedPtr(std::shared_ptr<T> shared)
    : std::shared_ptr<T>(std::move(shared))
{
}

template<typename T>
SharedPtr<T>::SharedPtr(std::unique_ptr<T>&& unique)
    : std::shared_ptr<T>(std::move(unique))
{
}

template<typename T>
template<typename M, typename X>
SharedPtr<M> SharedPtr<T>::appendLocal(M X::* localPointer) const
{
  return std::shared_ptr<M>(sliced(), &((**this).*localPointer));
}

template<typename T>
template<typename X>
SharedPtr<X> SharedPtr<T>::append(X* p) const
{
  return std::shared_ptr<X>(sliced(), p);
}

template<typename T>
template<typename M, std::enable_if_t<!std::is_void_v<M>, bool>>
T* SharedPtr<T>::operator->() const
{
  if (!*this) { throw std::bad_optional_access{}; }
  return std::shared_ptr<T>::operator->();
}

template<typename T>
template<typename M, std::enable_if_t<!std::is_void_v<M>, bool>>
M& SharedPtr<T>::operator*() const
{
  if (!*this) { throw std::bad_optional_access{}; }
  return std::shared_ptr<T>::operator*();
}

template<typename T>
WeakPtr<T> SharedPtr<T>::getWeakPtr() const
{
  return {sliced_nothrow()};
}

template<typename T>
const std::shared_ptr<T>& SharedPtr<T>::sliced() const
{
  if (!*this) { throw std::bad_optional_access{}; }
  return *this;
}

template<typename T>
template<typename S>
SharedPtr<S> SharedPtr<T>::cast() const
{
  if constexpr(std::is_same_v<T,S>) {
    return *this;
  } else if constexpr(std::has_virtual_destructor_v<T>) {
    return std::dynamic_pointer_cast<S>(sliced());
  } else {
    return std::static_pointer_cast<S>(sliced());
  }
}

template<typename T>
template<typename S>
SharedPtr<S> SharedPtr<T>::cast_nothrow() const
{
  if constexpr(std::is_same_v<T,S>) {
    return *this;
  } else if constexpr(std::has_virtual_destructor_v<T>) {
    return std::dynamic_pointer_cast<S>(sliced_nothrow());
  } else {
    return std::static_pointer_cast<S>(sliced_nothrow());
  }
}


template<typename T>
template<typename S>
WeakPtr<S> WeakPtr<T>::cast() const
{
  auto shared = lock();
  if(!shared) { return {}; }
  return shared.template cast<S>().getWeakPtr();
}




template<typename T>
JustLockPtr::JustLockPtr(const std::shared_ptr<T>& ptr) : lock(ptr)
{
  assert(lock && "You cannot hold an unlocked shared_ptr.");
}
