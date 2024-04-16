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

#ifndef ExpectedBehaviour_SharedPtr_impl_H
#define ExpectedBehaviour_SharedPtr_impl_H

#include "SharedPtr.h"  // Just to make tools happy!

#include <memory>
#include <optional>

template<typename T, typename NotBool>
SharedPtr<T, NotBool>::SharedPtr(const std::shared_ptr<T>& shared)
    : std::shared_ptr<T>(shared)
{
}

template<typename T, typename NotBool>
SharedPtr<T, NotBool>::SharedPtr(std::shared_ptr<T>&& shared)
    : std::shared_ptr<T>(std::move(shared))
{
}

template<typename T, typename NotBool>
template<typename X>
SharedPtr<T, NotBool>::SharedPtr(const SharedPtr<X>& shared, T X::* localPointer)
    : std::shared_ptr<T>(shared, &(shared->*localPointer))
{
}

template<typename T, typename NotBool>
template<typename X>
SharedPtr<T, NotBool>::SharedPtr(SharedPtr<X>&& shared, T X::* localPointer)
    : std::shared_ptr<T>(std::move(shared), &(shared->*localPointer))
{
}

template<typename T, typename NotBool>
T* SharedPtr<T, NotBool>::operator->() const
{
  if (!*this) { throw std::bad_optional_access{}; }
  return std::shared_ptr<T>::operator->();
}

template<typename T, typename NotBool>
T& SharedPtr<T, NotBool>::operator*() const
{
  if (!*this) { throw std::bad_optional_access{}; }
  return std::shared_ptr<T>::operator*();
}

template<typename T, typename NotBool>
SharedPtr<T, NotBool>::operator NotBool&() const
{
  if constexpr(std::is_same_v<NotBool, int>) {
    static int x = 0;
    return x;
  } else {
    return **this;
  }
}


template<typename T, typename NotBool>
WeakPtr<T> SharedPtr<T, NotBool>::getWeakPtr() const
{
  return WeakPtr<T>((const std::shared_ptr<T>&)(*this));
}

template<typename T, typename NotBool>
const std::shared_ptr<T>& SharedPtr<T, NotBool>::sliced() const
{
  if (!*this) { throw std::bad_optional_access{}; }
  return *this;
}

#endif  // ExpectedBehaviour_SharedPtr_impl_H
