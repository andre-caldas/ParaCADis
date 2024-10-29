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

#include <memory>

template<typename>
class SharedPtr;

template<typename>
class WeakPtr;

/**
 * Tailored std::enable_shared_from_this.
 * This class allows an object to provide a SharedPtr to itself.
 */
template<typename T>
class SelfShared
    : public std::enable_shared_from_this<T>
{
public:
  SharedPtr<T> getSelfShared();
  WeakPtr<T> getSelfWeak();

  /**
   * Convenience method to dynamic_cast and produce a SharedPtr.
   *
   * @attention
   * It is incorrect to provide a type to which this cannot be casted.
   */
  template<typename X>
  SharedPtr<X> getSelfShared();

  /**
   * Convenience method to dynamic_cast and produce a SharedPtr
   * to a local member.
   *
   * @attention
   * It is incorrect to provide a member of a type to which this cannot be casted.
   */
  template<typename X, typename C>
  SharedPtr<X> getLocalShared(X C::* local);
};

#include "SelfShared.hpp"
