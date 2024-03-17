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

#ifndef ExpectedBehaviour_SharedFromThis_H
#define ExpectedBehaviour_SharedFromThis_H

#include "SharedPtr.h"

#include <memory>

/**
 * A class with some improvements over std::enable_shared_from_this.
 */
template<typename Default>
class SharedFromThis
    : public std::enable_shared_from_this<Default>
{
public:
  virtual ~SharedFromThis() = default;

  template<typename X = Default>
  SharedPtr<X> sharedFromThis();
  template<typename X = Default>
  SharedPtr<const X> sharedFromThis() const;

  template<typename X = Default>
  WeakPtr<X> weakFromThis();
  template<typename X = Default>
  WeakPtr<const X> weakFromThis() const;
};

template<typename D>
template<typename X>
SharedPtr<X> SharedFromThis<D>::sharedFromThis()
{
  auto shared = this->shared_from_this();
  assert(shared);
  return std::static_pointer_cast<X>(std::move(shared));
}

template<typename D>
template<typename X>
SharedPtr<const X> SharedFromThis<D>::sharedFromThis() const
{
  auto shared = this->shared_from_this();
  assert(shared);
  return std::static_pointer_cast<const X>(std::move(shared));
}

template<typename D>
template<typename X>
WeakPtr<X> SharedFromThis<D>::weakFromThis()
{
  // We cannot cast a weak_ptr. :-(
  return sharedFromThis<X>();
}

template<typename D>
template<typename X>
WeakPtr<const X> SharedFromThis<D>::weakFromThis() const
{
  // We cannot cast a weak_ptr. :-(
  return sharedFromThis<const X>();
}

#endif  // ExpectedBehaviour_SharedFromThis_H

