// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2023 André Caldas <andre.em.caldas@gmail.com>            *
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

#include <atomic>
#include <memory>

/**
 * This class holds a constant and thread-safe object that is ALWAYS AVAILABLE.
 *
 * In a highly multithreaded environment,
 * we ensure that the generated data is encapsulated in an
 * atomic shared pointer to a constant data structure.
 * This way, threads that still rely on old data do not crash.
 * And no one needs to wait for the data to be generated,
 * because new data is generated "off line" and it is just
 * swapped with the official when it ready for use.
 */
template<typename T>
class AtomicHolder
{
public:
  SharedPtr<T> get() const;
  void set(const SharedPtr<T>& value);

private:
  std::atomic<std::shared_ptr<T>> ptr;
};

#include "AtomicHolder.hpp"
