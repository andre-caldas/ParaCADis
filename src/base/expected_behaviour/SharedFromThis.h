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

#include <memory>

/**
 * @brief A class with some improvements over std::enable_shared_from_this.
 */
template<typename Default>
class SharedFromThis
    : public std::enable_shared_from_this<SharedFromThis<Default>>
{
public:
  virtual ~SharedFromThis() = default;

  template<typename X = Default>
  std::shared_ptr<X> SharedFromThis();
  template<typename X = Default>
  std::shared_ptr<const X> SharedFromThis() const;

  template<typename X = Default>
  std::weak_ptr<X> WeakFromThis();
  template<typename X = Default>
  std::weak_ptr<const X> WeakFromThis() const;
};

template<typename D>
template<typename X>
std::shared_ptr<X> SharedFromThis<D>::SharedFromThis()
{
  auto shared = shared_from_this();
  assert(shared);
  return std::static_pointer_cast<X>(std::move(shared));
}

template<typename D>
template<typename X>
std::shared_ptr<const X> SharedFromThis<D>::SharedFromThis() const
{
  return std::static_pointer_cast<const X>(this->shared_from_this());
}

template<typename D>
template<typename X>
std::weak_ptr<X> SharedFromThis<D>::WeakFromThis()
{
  // We cannot cast a weak_ptr. :-(
  return SharedFromThis<X>();
}

template<typename D>
template<typename X>
std::weak_ptr<const X> SharedFromThis<D>::WeakFromThis() const
{
  // We cannot cast a weak_ptr. :-(
  return SharedFromThis<const X>();
}

#endif  // ExpectedBehaviour_SharedFromThis_H

