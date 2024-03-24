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

#ifndef ExpectedBehaviour_SharedPtr_H
#define ExpectedBehaviour_SharedPtr_H

#include <memory>

template<typename T>
class WeakPtr;

/**
 * @brief Safer to use shared_ptr.
 * Shall be used as the return value of a function
 * instead of a regular std::shared_ptr,
 * so the programmer (may, but) does not need to check
 * for the pointer validity.
 *
 * @example
 * // If you are confident the pointer is valid...
 * getPointer()->doStuff();  // Throws if the pointer is invalid.
 *
 * // Otherwise...
 * auto ptr = getPointer();
 * if(!ptr) {
 *     return;
 * }
 * ptr->doStuff();
 */
template<typename T>
class SharedPtr : public std::shared_ptr<T>
{
public:
  using value_type = T;
  SharedPtr()      = default;
  SharedPtr(const std::shared_ptr<T>& shared);
  SharedPtr(std::shared_ptr<T>&& shared);

  T* operator->() const;
  T& operator*() const;

  bool operator==(const SharedPtr<T>& other) const noexcept
  {
    // TODO: Why do I need "this"??? Remove and wee if compiler complains.
    return this->get() == other.get();
  }

  WeakPtr<T> getWeakPtr() const;

  const std::shared_ptr<T>& sliced() const;

  operator SharedPtr<const T>() const { return {sliced()}; }
};


template<typename T>
class WeakPtr : private std::weak_ptr<T>
{
public:
  using std::weak_ptr<T>::weak_ptr;
  WeakPtr(const SharedPtr<T>& shared) : WeakPtr(shared.getWeakPtr()) {}
  SharedPtr<T> lock() const noexcept { return std::weak_ptr<T>::lock(); }
};

#include "SharedPtr_impl.h"

#endif
