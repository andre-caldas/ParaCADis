// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2024 André Caldas <andre.em.caldas@gmail.com>            *
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

#pragma once

#include <base/expected_behaviour/SharedPtr.h>
#include <base/threads/locks/gates.h>

#include <concepts>

namespace Naming
{
  /**
   * Access to resolved names must comply with the "gate paradigm",
   * so its access becomes thread-safe.
   */
  template<typename T>
  class ResultHolder
  {
    template<typename S>
    friend class ResultHolder;

  private:
    Threads::MutexVector mutex;
    SharedPtr<T>         data;
    WeakPtr<T>           data_weak;

  public:
    ResultHolder() = default;

    /**
     * ResultHolder for data that holds a mutex.
     */
    ResultHolder(SharedPtr<T> _data)
    requires Threads::C_MutexHolder<T>
        : mutex(_data->getMutexLike())
        , data(std::move(_data))
        , data_weak(this->data)
    {}

    /**
     * ResultHolder for data and an explicit mutex.
     */
    template<Threads::C_MutexLike Mutex>
    ResultHolder(Mutex&& m, SharedPtr<T> _data)
        : mutex(std::forward<Mutex>(m))
        , data(std::move(_data))
        , data_weak(this->data)
    {}

    /**
     * ResultHolder for data that holds a mutex
     * but is not managed by a SharedPtr.
     */
    template<typename S>
      requires Threads::C_MutexHolder<T>
    ResultHolder(ResultHolder<S> parent, T* ptr)
        : mutex(ptr->getMutexLike())
        , data(parent.data.append(ptr))
        , data_weak(this->data)
    {}

    /**
     * ResultHolder for data that depends on some (parent) object
     * to provide a mutex and a SharedPtr.
     */
    template<typename S>
      requires (!Threads::C_MutexHolder<T>)
    ResultHolder(ResultHolder<S> parent, T* ptr)
        : mutex(parent.mutex)
        , data(parent.data.append(ptr)), data_weak(data)
    {}

    template<typename S>
    ResultHolder<S> cast() const
    {
      if(data) {
        auto cast_data = data.template cast<S>();
        if(!cast_data) {
          return {};
        }
        return ResultHolder<S>{mutex, cast_data, cast_data};
      }

      auto lock = data_weak.lock();
      if(!lock) {
        return {};
      }

      auto cast_data = lock.template cast<S>();
      if(!cast_data) {
        return {};
      }

      return ResultHolder<S>{mutex, {}, std::move(cast_data)};
    }

    operator bool() const { return bool(data); }
    bool operator==(const ResultHolder& other) const
    { return bool(data) && data == other.data; }

    constexpr auto& getMutexLike() const { return mutex; }

    using GateInfo = Threads::LocalGateInfo<&ResultHolder::data,
                                            &ResultHolder::mutex>;

    void releaseShared() { data.reset(); }
    bool lockShared()
    {
      if(data) { return true; }
      data = data_weak.lock();
      return bool(data);
    }

    ResultHolder getReleasedShared() const
    {
      ResultHolder result{*this};
      result.data.reset();
      return result;
    }

    ResultHolder getLockedShared() const
    {
      ResultHolder result{*this};
      result.data = data_weak.lock();
      return result;
    }

  private:
    ResultHolder(Threads::MutexVector v, SharedPtr<T> d, WeakPtr<T> w)
        : mutex(std::move(v))
        , data(std::move(d)), data_weak(std::move(w)) {}

  public:
    /**
     * ResultHolder is supposed to be accessed through a gate.
     * However, even when we have a gate, python expects to get
     * a SharedPtr and not a reference.
     */
    SharedPtr<T> _promiscuousGetShared() const
    {
      // TODO: if this assert is kept, remove the if and return const SharedPtr<T>&.
      assert(data && "Are you supposed to call this in a released state?");
      if(!data) {
        return data_weak.lock();
      }
      return data;
    }
  };

  static_assert(Threads::C_MutexHolderWithGates<ResultHolder<int>>,
                "Result gates is 'with gates'.");
}  // namespace Naming
