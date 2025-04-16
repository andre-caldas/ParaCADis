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

#include "reader_locks.h"

#include <concepts>

namespace Threads
{

  template<typename ItType, std::sentinel_for<ItType> Sentinel>
  class LockedIteratorSentinel
  {
  public:
    Sentinel end;
    LockedIteratorSentinel(Sentinel&& _end) : end(std::move(_end)) {}

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
    using difference_type = ItType::difference_type;
    using value_type      = ItType::value_type;
    using pointer         = ItType::pointer;
    using reference       = ItType::reference;

    template<std::sentinel_for<ItType> Sentinel>
    using sentinel_t = LockedIteratorSentinel<ItType, Sentinel>;
    using original_iterator_t = ItType;

    LockedIterator() = default;

    LockedIterator(const LockedIterator& other) = default;
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
    LockedIterator(SharedLock&& _lock, ItType _it)
        : originalIterator(std::move(_it))
        , lock(std::make_shared<SharedLock>(std::move(_lock)))
    {
    }

    LockedIterator& operator=(const LockedIterator& other) = default;
    LockedIterator& operator=(LockedIterator&& other) = default;

    LockedIterator& operator=(ItType it)
    {
      originalIterator = it;
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

    template<typename Sentinel>
    friend constexpr bool operator==(
        const Sentinel& sentinel, const LockedIterator& self)
    {
      return self == sentinel;
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

    /// We use a shared_ptr because we need copy-constructible iterator.
    std::shared_ptr<SharedLock> lock;
  };

  using test_it = std::vector<int>::iterator;
  static_assert(
      std::sentinel_for<LockedIteratorSentinel<test_it, test_it>, LockedIterator<test_it>>
                && "LockedIteratorSentinel must be a sentinel.");

  static_assert(std::input_or_output_iterator<LockedIterator<test_it>>
                && "LockedIterator must be an iterator.");

  static_assert(std::weakly_incrementable<LockedIterator<test_it>>
                && "LockedIterator must be (weakly) incrementable.");

}  // namespace Threads

#endif
