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

#include <libparacadis/base/threads/locks/LockedIterator.h>
#include <libparacadis/base/threads/locks/reader_locks.h>
#include <libparacadis/base/threads/locks/writer_locks.h>

namespace Threads::SafeStructs
{

  template<typename ContainerType>
  class ThreadSafeContainer
  {
  protected:
    mutable MutexData mutex;
    ContainerType     container;

  public:
    using self_t = ThreadSafeContainer;

    typedef ContainerType                               unsafe_container_t;
    typedef typename unsafe_container_t::iterator       container_iterator;
    typedef typename unsafe_container_t::const_iterator container_const_iterator;

    using iterator       = LockedIterator<container_iterator>;
    using const_iterator = LockedIterator<container_const_iterator>;

    using reference       = ContainerType::reference;
    using const_reference = ContainerType::const_reference;
    using value_type      = ContainerType::value_type;

    ThreadSafeContainer() = default;
    ThreadSafeContainer(int mutex_layer) : mutex(mutex_layer) {}

    auto begin();
    auto begin() const;
    auto cbegin() const;

    auto end();
    auto end() const;
    auto cend() const;

    size_t size() const;
    bool   empty() const;
    void   clear();


    using GateInfo = Threads::LocalGateInfo<&self_t::container,
                                            &self_t::mutex>;

    constexpr auto& getMutexLike() const { return mutex; }
  };

}  // namespace Threads::SafeStructs

#include "ThreadSafeContainer_inl.h"

#endif  // SafeStructs_ThreadSafeContainer_H
