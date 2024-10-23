// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2023-2024 André Caldas <andre.em.caldas@gmail.com>       *
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

#include "ThreadSafeSharedPtr.h"

namespace Threads::SafeStructs
{

template<typename T>
ThreadSafeSharedPtr<T>::ThreadSafeSharedPtr(SharedPtr<T> shared_ptr)
    : theSharedPtr(std::move(shared_ptr))
{}

template<typename T>
SharedPtr<T>
ThreadSafeSharedPtr<T>::getSharedPtr() const
{
  Threads::SharedLock lock{mutex};
  return theSharedPtr;
}

template<typename T>
SharedPtr<T>
ThreadSafeSharedPtr<T>::setSharedPtr(SharedPtr<T> shared_ptr)
{
  Threads::ExclusiveLock lock{mutex};
  SharedPtr<T> result = std::move(theSharedPtr);
  theSharedPtr = std::move(shared_ptr);
  return result;
}

}  // namespace Threads::SafeStructs
