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

#ifndef ThreadSafeStructs_ThreadSafeMap_H
#define ThreadSafeStructs_ThreadSafeMap_H

#include "ThreadSafeContainer.h"

#include <map>
#include <unordered_map>

namespace ThreadSafeStructs
{

  /**
   * @brief This wraps an std::map or std::unordered_map and provides a
   * shared_mutex to make the map readable by many only when the map structure
   * is not being modified.
   * The map elements are not protected, just the map structure.
   * To protect each element, use std::atomic.
   *
   * Iterators use a shared_lock.
   * Methods that change the map structure use a lock.
   */
  template<typename MapType, typename Key, typename Val>
  class ThreadSafeMapCommon : public ThreadSafeContainer<MapType>
  {
  public:
    using self_t   = ThreadSafeMapCommon;
    using parent_t = ThreadSafeContainer<MapType>;
    using typename parent_t::const_iterator;
    using typename parent_t::iterator;

    using parent_t::ThreadSafeContainer;

    auto find(const Key& key)
    {
      return LockedIterator(mutex, container.find(key));
    }

    auto find(const Key& key) const
    {
      return LockedIterator(mutex, container.find(key));
    }

    size_t count(const Key& key) const
    {
      SharedLockFreeLock l(mutex);
      return container.count(key);
    }

    bool contains(const Key& key) const
    {
      SharedLockFreeLock l(mutex);
      return container.count(key);
    }

  protected:
    using parent_t::container;
    using parent_t::mutex;
  };

  /**
   * @brief This wraps an std::map and provides a shared_mutex
   * to make the map readable by many only when the map structure
   * is not being modified.
   * The map elements are not protected, just the map structure.
   * To protect each element, use std::atomic.
   *
   * Iterators use a shared_lock.
   * Methods that change the map structure use a lock.
   */
  template<typename Key, typename Val>
  class ThreadSafeMap : public ThreadSafeMapCommon<std::map<Key, Val>, Key, Val>
  {
  };

  /**
   * @brief This wraps an std::map and provides a shared_mutex
   * to make the map readable by many only when the map structure
   * is not being modified.
   * The map elements are not protected, just the map structure.
   * To protect each element, use std::atomic.
   *
   * Iterators use SharedLock.
   * Methods that change the map structure use a lock.
   */
  template<typename Key, typename Val>
  class ThreadSafeUnorderedMap
      : public ThreadSafeMapCommon<std::unordered_map<Key, Val>, Key, Val>
  {
  };

}  // namespace ThreadSafeStructs

#endif  // ThreadSafeStructs_ThreadSafeMap_H
