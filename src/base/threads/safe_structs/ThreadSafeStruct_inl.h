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

#include "ThreadSafeStruct.h"

namespace Threads::SafeStructs
{

template<typename Struct>
template<typename... Args>
ThreadSafeStruct<Struct>::ThreadSafeStruct(Args&&... args)
    : theStruct(args...)
{}

template<typename Struct>
ThreadSafeStruct<Struct>::~ThreadSafeStruct()
{
    /*
     * This is very subtle...
     * Usually, a shared_ptr that holds the structure that holds
     * this ThreadSafeStruct is passed to the dedicatedThread.
     * This means that:
     *
     * 1. We cannot join() on destruction,
     * because we might be joining from the thread we want to join to!
     *
     * 2. We do not need to join, because the all the lambdas have certainly
     * finished processing.
     */
    if (dedicatedThread.joinable()) {
        dedicatedThread.detach();
    }
}

#if 0 ///// TODO: move that code to Gate.
template<typename Struct>
auto ThreadSafeStruct<Struct>::lockForReading() const
{
    return r_lock_t {*this};
}

template<typename Struct>
auto ThreadSafeStruct<Struct>::continueReading()
{
    w_lock_t lock {*this, true};
    if (lock) {
        lock.release();
        lock.resumeReading();
    }
    return lock;
}


template<typename Struct>
auto ThreadSafeStruct<Struct>::startWriting()
{
    return w_lock_t {*this};
}

template<typename Struct>
auto ThreadSafeStruct<Struct>::continueWriting()
{
    return w_lock_t {*this, true};
}

template<typename Struct>
void ThreadSafeStruct<Struct>::cancelThreads()
{
    activeThread = std::thread::id {};
}

template<typename Struct>
std::thread& ThreadSafeStruct<Struct>::getDedicatedThread()
{
    return dedicatedThread;
}
#endif

}  // namespace Threads::SafeStructs
