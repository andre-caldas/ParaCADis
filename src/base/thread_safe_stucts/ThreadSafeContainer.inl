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

#include "LockedIterator.h"
#include "ThreadSafeContainer.h"
#include "locks/LockPolicy.h"

#include <mutex>

namespace ThreadSafeStructs
{

  /*
   * Iterator: begin().
   */
  template<typename ContainerType>
  auto ThreadSafeContainer<ContainerType>::begin()
  {
    return iterator(mutex, container.begin());
  }

  template<typename ContainerType>
  auto ThreadSafeContainer<ContainerType>::begin() const
  {
    return cbegin();
  }

  template<typename ContainerType>
  auto ThreadSafeContainer<ContainerType>::cbegin() const
  {
    return const_iterator(mutex, container.cbegin());
  }

  /*
   * Iterator: end().
   */
  template<typename ContainerType>
  auto ThreadSafeContainer<ContainerType>::end()
  {
    return iterator::MakeEndIterator(container.end());
  }

  template<typename ContainerType>
  auto ThreadSafeContainer<ContainerType>::end() const
  {
    return cend();
  }

  template<typename ContainerType>
  auto ThreadSafeContainer<ContainerType>::cend() const
  {
    return const_iterator::MakeEndIterator(container.cend());
  }

  template<typename ContainerType>
  size_t ThreadSafeContainer<ContainerType>::size() const
  {
    SharedLockFreeLock lock(mutex);
    return container.size();
  }

  template<typename ContainerType>
  bool ThreadSafeContainer<ContainerType>::empty() const
  {
    SharedLockFreeLock lock(mutex);
    return container.empty();
  }

  template<typename ContainerType>
  void ThreadSafeContainer<ContainerType>::clear()
  {
    ExclusiveLockFreeLock lock(*this);
    container.clear();
  }

  template<typename ContainerType>
  template<typename SomeHolder>
  void ThreadSafeContainer<ContainerType>::setParentMutex(SomeHolder& tsc)
  {
    auto gate         = tsc.getWriterGate(this);
    mutex.parent_pair = gate.getMutexPtr();
  }

}  // namespace ThreadSafeStructs

