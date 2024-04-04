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

#ifndef Threads_LockedIterator_H
#define Threads_LockedIterator_H

#include "LockPolicy.h"

#include <iterator>

namespace Threads
{

  template<typename ItType, std::sentinel_for<ItType> Sentinel>
  class LockedIteratorSentinel
  {
  public:
    Sentinel end;
    LockedIteratorSentinel(Sentinel&& end) : end(std::move(end)) {}

    LockedIteratorSentinel() = default;
    LockedIteratorSentinel(const LockedIteratorSentinel&) = default;
    LockedIteratorSentinel(LockedIteratorSentinel&&) = default;

    LockedIteratorSentinel& operator=(const LockedIteratorSentinel&) = default;
    LockedIteratorSentinel& operator=(LockedIteratorSentinel&&) = default;
  };

  template<typename ItType>
  class LockedIterator
  {
  public:
    template<std::sentinel_for<ItType> Sentinel>
    using sentinel_t = LockedIteratorSentinel<ItType, Sentinel>;
    using original_iterator_t = ItType;

    // I hope nobody uses this. Some algorithms may fail. Non-portable.
    LockedIterator() = default;

    // Attention: do not lock mutex again!
    LockedIterator(const LockedIterator& other) = delete;
    LockedIterator(LockedIterator&&) = default;

    /**
     * Operations that return an iterator (to be made LockedIterator),
     * they do need a lock before doing the search.
     *
     * The SharedLock ownership is transfered to the LockedIterator
     * so it can live as long as the iterator itself.
     * @param lock - the SharedLock instance.
     * @param it - original iterator to be wrapped.
     * @attention It might happen that the SharedLock is dummy because
     * some outter context has already locked it. It is very important that
     * the iterator does not live longer than the real lock.
     */
    LockedIterator(SharedLock&& lock, ItType it)
        : originalIterator(std::move(it)), lock(std::move(lock))
    {
    }

    // We need LockedIterator to be copy assignable to use algorithms.
    // But we shall never use it. We do not change mutexes, only the iterator.
    // So, this messes with LockPolicy.
    // I hope nobody uses this. Some algorithms may fail. Non-portable.
    LockedIterator& operator=(const LockedIterator& other)
    {
      assert(false && "It is a bad idea to assign locks.");
      originalIterator = other.originalIterator;
      return *this;
    }

    constexpr bool operator==(const LockedIterator& other) const
    {
      return originalIterator == other.originalIterator;
    }

    // TODO: It seems LockedIteratorSentinel<> is NOT a sentinel after all. :-(
//    template<std::sentinel_for<ItType> Sentinel>
    template<typename Sentinel>
    constexpr bool operator==(const Sentinel& sentinel) const
    {
      return originalIterator == sentinel.end;
    }

    LockedIterator& operator++()
    {
      ++originalIterator;
      return *this;
    }

    LockedIterator operator++(int)
    {
      LockedIterator result{*this};
      ++originalIterator;
      return result;
    }

    auto& operator*()
    {
      return *originalIterator;
    }

    auto operator->()
    {
      return originalIterator.operator->();
    }

    operator const ItType&() const { return originalIterator; }
    operator ItType&() { return originalIterator; }

  private:
    ItType originalIterator;
    mutable SharedLock lock;
  };

}  // namespace Threads

#endif
