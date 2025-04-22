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

#ifndef BASE_Threads_NewThreadLock_H
#define BASE_Threads_NewThreadLock_H

#include <memory>

#include <libparacadis/base/type_traits/Utils.h>

#include "LockPolicy.h"

namespace Threads
{

/**
 * Locks a classes of type "MutexHolder" for "writing".
 *
 * This is for heavy processing and allows release and resume.
 * The constructor is passed a "token" and the token is always checked
 * to see if there is no "newer" thread processing the same data.
 * If there is a newer thread, the lock cannot be resumed.
 * The MutexHolder must satisfy the requirements imposed by ExclusiveLock.
 *
 * @attention Exclusive locks must be acquired all at once.
 */
template<typename MutexHolder>
class NewThreadLock<MutexHolder>
{
public:
    [[nodiscard]]
    NewThreadLock(MutexHolder& mutex_holder, bool try_to_resume = false);

    /**
     * We allow move constructor only when already "locked".
     *
     * That is, not after a "release()".
     * We use it to pass the lock to a lambda closure, to start a new thread.
     * Otherwise, it is better to pass a reference to the lock.
     * @param other: NewThreadLock to move.
     * @attention This is to be used ONLY to pass the lock to a new thread.
     */
    [[nodiscard]]
    NewThreadLock(NewThreadLock<MutexHolder>&& other);


    /**
     * Checks if this processing block is outdated.
     * @return No newer processing started: false. Otherwise, true.
     */
    bool isThreadObsolete() const;

    /**
     * Releases the lock.
     * This allows other processes to:
     * 1. Read the data.
     * 2. Start a new processing, making this instance "outdated".
     */
    void release();

    /**
     * Acquires an exclusive lock and resumes processing.
     * @return Resume success: isValid().
     */
    bool resume();

    /**
     * Resumes using a shared lock instead of an exclusive lock.
     * @attention The gate is a WriterGate.
     * @return Resume success: isValid().
     */
    bool resumeReading();

    /**
     * Start a new thread and moves the mutex to this newly created thread.
     * @param f: Callable.
     * @param args: Arguments to the callable.
     */
    template<class Function, class... Args>
    void startNewThread(Function&& f, Args&&... args) &&;

    /**
     * Passes the ownership to a new thread.
     * Otherwise, resume() will fail.
     *
     * @see releaseFromThread().
     * @see resume().
     *
     * @attention Thread shall not have any other locks.
     *
     * @example
     * lock.moveFromThread();
     * auto lambda = [lock = std::move(lock)] {
     *   lock.resumeInNewThread();
     *   // Use "lock" or simply release it.
     * };
     * std::thread(lambda);
     */
    void resumeInNewThread();

    /**
     * Just like "resumeInNewThread()", but it also releases the lock.
     * This is just a shortcut so the code becomes cleaner.
     *
     * @see resumeInNewThread();
     */
    void releaseInNewThread();

    auto& operator->() const;

    /**
     * Indicates if we shall continue processing: !isThreadObsolete().
     * @attention In general, true does not mean the resource is locked.
     * But it does mean so after the (non-move) construction.
     *
     * @example
     * auto lock = struct.continueWriting();
     * if(!lock) {
     *   return;
     * }
     * // Use the lock.
     */
    operator bool() const;

private:
    std::unique_ptr<ExclusiveLock<MutexHolder>> exclusiveLock;
    std::unique_ptr<SharedLock> sharedLock;

    MutexHolder& mutexHolder;
    const typename MutexHolder::WriterGate& gate;

    /**
     * The idea is to allow for the write lock to be released and acquired again.
     *
     * We register the time when the processing "block" started so we can:
     * 1. Decide on which processed data is newer.
     * 2. Discard outdated results.
     * 3. Cancel threads that are "old".
     *
     * @attention Whenever the lock is released
     * the data shall be in a consistent state.
     */
    void markStart();

    /**
     * First step in passing the ownership from the current thread.
     *
     * This is a little low level. Consider using startNewThread() instead.
     *
     * @return A shared_ptr to the exclusive mutex,
     * so that the calling context can hold the lock
     * to save the thread information (i.e.: std::tread).
     *
     * @attention Needs to be exclusively locked.
     * Cannot be locked prior to this instance.
     *
     * @see resumeFromThread().
     */
    [[maybe_unused]]
    auto moveFromThread();
};

}  // namespace Base::Threads

#include "NewThreadLock_inl.h"

#endif  // BASE_Threads_NewThreadLock_H
