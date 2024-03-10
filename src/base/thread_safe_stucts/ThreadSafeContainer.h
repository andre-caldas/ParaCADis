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

#ifndef ThreadSafeStructs_ThreadSafeContainer_H
#define ThreadSafeStructs_ThreadSafeContainer_H

#include <type_traits>
#include <shared_mutex>

#include <base/threads/locks/LockPolicy.h>

namespace ThreadSafeStructs
{

template<typename ItType>
class LockedIterator;

template<typename ContainerType>
class ThreadSafeContainer
{
public:
    using self_t = ThreadSafeContainer;
    typedef ContainerType unsafe_container_t;
    typedef typename unsafe_container_t::iterator container_iterator;
    typedef typename unsafe_container_t::const_iterator container_const_iterator;

    typedef LockedIterator<container_iterator> iterator;
    typedef LockedIterator<container_const_iterator> const_iterator;

    ThreadSafeContainer() = default;

    template<typename MutexHolder>
    ThreadSafeContainer(MutexHolder& holder) : mutex(holder.getMutexPair()) {}

    auto begin();
    auto begin() const;
    auto cbegin() const;

    auto end();
    auto end() const;
    auto cend() const;

    size_t size() const;
    bool empty() const;
    void clear();

    struct WriterGate
    {
        WriterGate(self_t* self) : self(self) {}
        self_t* self;
        auto operator->() const {return &self->container;}
        auto& operator*() const {return self->container;}
    };
    WriterGate& getWriterGate(const ExclusiveLockBase*)
    {assert(LockPolicy::isLockedExclusively(mutex)); return gate;}

    template<typename SomeHolder>
    void setParentMutex(SomeHolder& tsc);

public:
    // TODO: eliminate this or the gate version.
    auto getMutexPair() const {return &mutex;}

protected:
    WriterGate gate{this};

    mutable MutexPair mutex;
    ContainerType container;
};

} //namespace ThreadSafeStructs

#include "ThreadSafeContainer.inl"

#endif // ThreadSafeStructs_ThreadSafeContainer_H