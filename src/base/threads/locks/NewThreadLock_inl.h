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

#include "NewThreadLock.h"
#include "exceptions.h"

#include <memory>
#include <utility>

namespace Threads
{

  template<typename MutexHolder>
  NewThreadLock<MutexHolder>::NewThreadLock(MutexHolder& mutex_holder, bool try_to_resume)
      : exclusiveLock(std::make_unique<ExclusiveLock<MutexHolder>>(mutex_holder))
      , mutexHolder(mutex_holder)
      , gate(mutex_holder.getWriterGate(exclusiveLock.get()))
  {
    assert(!sharedLock && exclusiveLock);

    if (try_to_resume) {
      if (isThreadObsolete()) { release(); }
    } else {
      markStart();
    }
  }

  template<typename MutexHolder>
  NewThreadLock<MutexHolder>::NewThreadLock(NewThreadLock<MutexHolder>&& other)
      : exclusiveLock(std::move(other.exclusiveLock))
      , sharedLock(std::move(other.sharedLock))
      , mutexHolder(other.mutexHolder)
      , gate(other.gate)
  {
    // Makes sure it is locked.
    assert(exclusiveLock);
    // Makes sure it was moved from thread: moveFromThread().
    assert(mutexHolder.activeThread == std::thread::id{});
  }


  template<typename MutexHolder>
  bool NewThreadLock<MutexHolder>::isThreadObsolete() const
  {
    return mutexHolder.activeThread != std::this_thread::get_id();
  }

  template<typename MutexHolder>
  void NewThreadLock<MutexHolder, nullptr>::release()
  {
    if (!sharedLock && !exclusiveLock) { throw Exception::CannotReleaseUnlocked{}; }
    assert(!sharedLock || !exclusiveLock);
    assert(sharedLock || exclusiveLock);
    exclusiveLock.reset();
    sharedLock.reset();
  }

  template<typename MutexHolder>
  bool NewThreadLock<MutexHolder>::resume()
  {
    assert(!sharedLock && !exclusiveLock);
    exclusiveLock = std::make_unique<ExclusiveLock<MutexHolder>>(mutexHolder);
    if (isThreadObsolete()) {
      release();
      return false;
    }
    return true;
  }

  template<typename MutexHolder>
  bool NewThreadLock<MutexHolder>::resumeReading()
  {
    assert(!sharedLock && !exclusiveLock);
    sharedLock = std::make_unique<SharedLock>(mutexHolder.getMutexData());
    if (isThreadObsolete()) {
      release();
      return false;
    }
    return true;
  }

  template<typename MutexHolder>
  auto& NewThreadLock<MutexHolder>::operator->() const
  {
    assert(!exclusiveLock || !sharedLock);
    assert(exclusiveLock || sharedLock);
    if (!exclusiveLock && !sharedLock) { throw Exception::NeedLock{}; }
    // TODO: somehow, use reader_gate for sharedLock.
    return gate;
  }

  template<typename MutexHolder>
  NewThreadLock<MutexHolder>::operator bool() const
  {
    return !isThreadObsolete();
  }

  template<typename MutexHolder>
  template<class Function, class... Args>
  void NewThreadLock<MutexHolder>::startNewThread(Function&& f, Args&&... args) &&
  {
    if (mutexHolder.dedicatedThread.joinable()) {
      assert(false);
      throw Exception::NoNestedThreads{};
    }

    // Hold the lock while we set mutexHolder.dedicatedThread.
    auto hold                   = moveFromThread();
    mutexHolder.dedicatedThread = std::thread{
        std::forward<Function>(f), std::move(*this), std::forward<Args>(args)...};
  }


  template<typename MutexHolder>
  void NewThreadLock<MutexHolder>::markStart()
  {
    ExclusiveLock l{mutexHolder};
    mutexHolder.activeThread = std::this_thread::get_id();
    if (mutexHolder.dedicatedThread.joinable()) { mutexHolder.dedicatedThread.detach(); }
  }

  template<typename MutexHolder>
  auto NewThreadLock<MutexHolder>::moveFromThread()
  {
    if (!exclusiveLock) {
      assert(false);
      throw Exception::NewThreadRequiresLock{};
    }
    assert(!sharedLock);

    // Prevent this thread from hijacking back the lock.
    // Makes bool(lock) evaluate to false;
    mutexHolder.activeThread = std::thread::id{};
    return exclusiveLock->detachFromThread();
  }

  template<typename MutexHolder>
  void NewThreadLock<MutexHolder>::resumeInNewThread()
  {
    if (!exclusiveLock) {
      assert(false);
      throw Exception::NewThreadRequiresLock{};
    }

    if (std::thread::id{} != mutexHolder.activeThread) {
      assert(false);
      throw Exception::NewThreadRequiresMovedLock{};
    }

    // Prevent this thread from hijacking (continueWriting) it back.
    mutexHolder.activeThread = std::this_thread::get_id();
    exclusiveLock->attachToThread(true);
  }

  template<typename MutexHolder>
  void NewThreadLock<MutexHolder>::releaseInNewThread()
  {
    resumeInNewThread();
    release();
  }

}  // namespace Threads
