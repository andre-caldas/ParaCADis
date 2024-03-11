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

#include <shared_mutex>
#include <type_traits>

namespace Threads
{

  template<typename ItType>
  class LockedIterator : private ItType
  {
  public:
    using value_type        = typename ItType::value_type;
    using difference_type   = typename ItType::difference_type;
    using pointer           = typename ItType::pointer;
    using reference         = typename ItType::reference;
    using iterator_category = typename ItType::iterator_category;

    // Attention: do not lock mutex again!
    LockedIterator(const LockedIterator& other) : ItType(other) {}

    /**
     * @brief An iterator (wrapper) that locks the mutex using SharedLock.
     * @param mutex - the mutex to lock.
     * @param it - original iterator to be wrapped.
     */
    LockedIterator(MutexPair& mutex, ItType it)
        : ItType(std::move(it))
        , lock(mutex)
    {
    }

    // We need LockedIterator to be copy assignable to use algorithms.
    // But we shall never use it. We do not change mutexes, only the iterator.
    // So, this messes with LockPolicy.
    LockedIterator& operator=(const LockedIterator& other)
    {
      assert(false);
      ItType::operator=(other);
      return *this;
    }

    constexpr bool operator==(const LockedIterator& other) const
    {
      return ItType::operator==(other);
    }
    constexpr bool operator!=(const LockedIterator& other) const
    {
      return ItType::operator!=(other);
    }

    LockedIterator& operator++()
    {
      ItType::operator++();
      return *this;
    }
    LockedIterator operator++(int)
    {
      LockedIterator result(*this);
      ++*this;
      return result;
    }

    using ItType::operator==;
    using ItType::operator!=;
    using ItType::operator*;
    using ItType::operator->;

    /**
     * @brief Static method to provide a LockedIterator that does not lock
     * anything. It shall be used only to construct and "end" iterator.
     * @param end_it - original container's end iterator.
     * @return An "end" iterator of type LockedIterator<ItType>.
     */
    static LockedIterator<ItType> MakeEndIterator(ItType&& end_it)
    {
      return LockedIterator<ItType>(std::move(end_it));
    }

    operator const ItType&() const { return *this; }
    operator ItType&() { return *this; }

  private:
    mutable SharedLockFreeLock lock;

    /**
     * @brief A wrapper that does not actually lock anything.
     * Shall be used only for the "end()" iterator.
     * @param it - the "end()" iterator to be wrapped.
     * @attention It is better if the end() iterator and the other iterators
     * are of the same type.
     * Therefore, we also wrap the "end()" iterator.
     * But, since it might be short lived, we do not want the lock to be tied to it.
     */
    explicit LockedIterator(ItType&& it) : ItType(std::move(it)) {}
  };

}  // namespace Threads

#endif  // Threads_LockedIterator_H

