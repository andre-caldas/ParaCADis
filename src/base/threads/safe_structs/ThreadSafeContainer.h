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

#ifndef SafeStructs_ThreadSafeContainer_H
#define SafeStructs_ThreadSafeContainer_H

#include <base/threads/locks/LockedIterator.h>
#include <base/threads/locks/reader_locks.h>
#include <base/threads/locks/writer_locks.h>

namespace Threads::SafeStructs
{

  template<typename ContainerType>
  class ThreadSafeContainer
  {
  protected:
    mutable Threads::MutexData defaultMutex;
    Threads::MutexData&        mutex = defaultMutex;
    ContainerType              container;

  public:
    using self_t = ThreadSafeContainer;
    using mutex_data_t = MutexData;

    typedef ContainerType                         unsafe_container_t;
    typedef typename unsafe_container_t::iterator container_iterator;
    typedef typename unsafe_container_t::const_iterator container_const_iterator;

    typedef LockedIterator<container_iterator>       iterator;
    typedef LockedIterator<container_const_iterator> const_iterator;

    ThreadSafeContainer() = default;

    template<C_MutexHolder MutexHolder>
    ThreadSafeContainer(MutexHolder& holder)
        : mutex(holder)
    {
    }

    template<C_MutexHolder MutexHolder>
    ThreadSafeContainer(int mutex_layer)
        : defaultMutex(mutex_layer)
    {
    }

    auto begin();
    auto begin() const;
    auto cbegin() const;

    auto end();
    auto end() const;
    auto cend() const;

    size_t size() const;
    bool   empty() const;
    void   clear();


    using ReaderGate = ::ReaderGate<&self_t::container>;
    using WriterGate = ::WriterGate<&self_t::container>;

    ReaderGate getReaderGate() const noexcept { return ReaderGate{*this}; }
    WriterGate getWriterGate() noexcept { return WriterGate{*this}; }

    template<typename SomeHolder>
    void setParentMutex(SomeHolder& tsc);

  public:
    constexpr operator MutexData&() const { return mutex; }
    constexpr MutexData& getMutexData() const { return mutex; }
  };

}  // namespace Threads::SafeStructs

#include "ThreadSafeContainer_inl.h"

#endif  // SafeStructs_ThreadSafeContainer_H
