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

#ifndef ExpectedBehaviour_SharedPtr_H
// The following error is because we do not want to incentivate developers
// to use "SharedPtr_impl.h" instead of "SharedPtr.h".
#  error "Include SharedPtr.h before including SharedPtr_impl.h. Do you need 'impl'?"
#endif
#include "SharedPtr.h"  // Just to make tools happy!

#include <memory>
#include <optional>

template<typename T>
SharedPtr<T>::SharedPtr(const std::shared_ptr<T>& shared)
    : std::shared_ptr<T>(shared)
{
}

template<typename T>
SharedPtr<T>::SharedPtr(std::shared_ptr<T>&& shared)
    : std::shared_ptr<T>(std::move(shared))
{
}

template<typename T>
T* SharedPtr<T>::operator->() const
{
  if (!*this) { throw std::bad_optional_access{}; }
  return std::shared_ptr<T>::operator->();
}

template<typename T>
T& SharedPtr<T>::operator*() const
{
  if (!*this) { throw std::bad_optional_access{}; }
  return std::shared_ptr<T>::operator*();
}

template<typename T>
WeakPtr<T> SharedPtr<T>::getWeakPtr() const
{
  return WeakPtr<T>((const std::shared_ptr<T>&)(*this));
}

template<typename T>
const std::shared_ptr<T>& SharedPtr<T>::sliced() const
{
  if (!*this) { throw std::bad_optional_access{}; }
  return *this;
}

#endif  // ExpectedBehaviour_SharedPtr_impl_H

