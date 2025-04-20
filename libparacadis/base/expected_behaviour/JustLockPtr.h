// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2024-2025 André Caldas <andre.em.caldas@gmail.com>       *
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

#include "SharedPtr.h"

#include <memory>

/**
 * When you just want to be sure the data will be kept valid.
 */
class JustLockPtr
{
public:
  JustLockPtr() = default;
  JustLockPtr(const JustLockPtr&) = default;
  JustLockPtr(JustLockPtr&&) = default;

  JustLockPtr& operator= (const JustLockPtr& other);
  JustLockPtr& operator= (JustLockPtr&& other);

  JustLockPtr(std::shared_ptr<void>&& ptr);

  template<typename T>
  JustLockPtr(const std::shared_ptr<T>& ptr);

  template<typename T>
  JustLockPtr(const SharedPtr<T>& ptr) : JustLockPtr(ptr.sliced()) {}

  operator bool() const {return bool(lock);}
  void* getVoidPtr() const {return lock.get();}

private:
  std::shared_ptr<void> lock;
};


/**
 * When you want to be able to check if the data is still valid
 * and keep it valid when needed.
 */
class JustLockWeak
{
public:
  JustLockWeak() = default;
  JustLockWeak(const JustLockWeak&) = default;
  JustLockWeak(JustLockWeak&&) = default;

  JustLockWeak& operator= (const JustLockWeak&) = default;
  JustLockWeak& operator= (JustLockWeak&&) = default;

  template<typename T>
  JustLockWeak(const std::shared_ptr<T>& ptr) : weak(ptr) {}

  template<typename T>
  JustLockWeak(const SharedPtr<T>& ptr) : JustLockWeak(ptr.sliced()) {}

  JustLockPtr lock() const {return weak.lock();}

private:
  std::weak_ptr<void> weak;
};

#include "JustLockPtr.hpp"
