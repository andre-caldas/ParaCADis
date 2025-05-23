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

#include <memory>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace pybind11 {
  template <typename type_, typename... options>
  class class_;
}

template<typename T>
class WeakPtr;

// TODO: move this somewhere else.
template<typename S, typename T>
concept C_DynamicCastableTo = requires(S* p)
{ dynamic_cast<T*>(p); };

/**
 * Safer to use shared_ptr.
 * Shall be used as the return value of a function
 * instead of a regular std::shared_ptr,
 * so the programmer (may, but) does not need to check
 * for the pointer validity.
 *
 * @example
 * // If you are confident the pointer is valid...
 * getPointer()->doStuff();  // Throws if the pointer is invalid.
 *
 * // Otherwise...
 * auto ptr = getPointer();
 * if(!ptr) {
 *     return;
 * }
 * ptr->doStuff();
 */
template<typename T>
class SharedPtr : private std::shared_ptr<T>
{
public:
  using value_type = T;
  SharedPtr() = default;
  SharedPtr(const SharedPtr&) = default;
  SharedPtr(SharedPtr&&) = default;
  SharedPtr(std::shared_ptr<T> shared);
  SharedPtr(std::unique_ptr<T>&& unique);

  /**
   * The pybind11 library needs this constructor to support conversion
   * of types managed by SharedPtr. For example, to pass a
   * SharedPtr<DeferenceablePoint> to a method that takes a
   * SharedPtr<ExporterCommon>, this implicit conversion is necessary.
   * The library only performs the conversion if SharedPtr implements
   * this constructor.
   */
  template<typename X>
  SharedPtr(const SharedPtr<X>& base, T* ptr) : std::shared_ptr<T>(base, ptr) {}

  template<typename M, typename X>
  SharedPtr<M> appendLocal(M X::* localPointer) const;
  template<typename X>
  SharedPtr<X> append(X* p) const;

  using std::shared_ptr<T>::operator bool;
  using std::shared_ptr<T>::operator=;
  using std::shared_ptr<T>::get;
  using std::shared_ptr<T>::reset;
  using std::shared_ptr<T>::swap;

  SharedPtr& operator=(const SharedPtr&) = default;
  SharedPtr& operator=(SharedPtr&&) = default;

  template<typename M = T, std::enable_if_t<!std::is_void_v<M>, bool> = true>
  M& operator*() const;
  template<typename M = T, std::enable_if_t<!std::is_void_v<M>, bool> = true>
  T* operator->() const;

  template<typename... Args>
  static SharedPtr<T> make_shared(Args&&... args);

  static SharedPtr<T> from_pointer(T* ptr);

  template<typename S>
  bool operator==(const SharedPtr<S>& other) const noexcept
  {
    return dynamic_cast<void*>(this->get()) == dynamic_cast<void*>(other.get());
  }

  WeakPtr<T> getWeakPtr() const;

  const std::shared_ptr<T>& sliced() const;
  const std::shared_ptr<T>& sliced_nothrow() const {return *this;}

  operator SharedPtr<const T>() const { return {sliced()}; }
  template<typename S>
    requires std::convertible_to<T&, S&>
  operator SharedPtr<S>() const { return {sliced()}; }

  template<typename S>
  requires C_DynamicCastableTo<T, S>
  SharedPtr<S> cast() const;
//  template<typename S>
//  SharedPtr<S> cast() const;

  template<typename S>
  requires C_DynamicCastableTo<T, S>
  SharedPtr<S> cast_nothrow() const;
//  template<typename S>
//  SharedPtr<S> cast_nothrow() const;

private:
  explicit SharedPtr(T* ptr) : std::shared_ptr<T>(ptr) {}
  template <typename type_, typename... options>
  friend class pybind11::class_;

  template <typename>
  friend class SharedPtr;
};

namespace std {
  template<typename T>
  struct hash<SharedPtr<T>> {
    size_t operator()(SharedPtr<T> p) {return p.get();}
  };
}


template<typename T>
class WeakPtr : private std::weak_ptr<T>
{
public:
  using std::weak_ptr<T>::weak_ptr;
  WeakPtr(const WeakPtr&) = default;
  WeakPtr(WeakPtr&&) = default;
  WeakPtr& operator=(const WeakPtr&) = default;
  WeakPtr& operator=(WeakPtr&&) = default;

  WeakPtr(const SharedPtr<T>& shared) : WeakPtr(shared.getWeakPtr()) {}
  SharedPtr<T> lock() const noexcept { return std::weak_ptr<T>::lock(); }

  template<typename S>
  WeakPtr<S> cast() const;
};

#include "SharedPtr.hpp"
