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

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

#include <base/expected_behaviour/SharedPtr.h>
#include <base/naming/Uuid.h>

PYBIND11_MAKE_OPAQUE(Naming::Uuid::uuid_type);

/*
 * A SharedPtr<T> is copy constructible even if T is not!
 * So, PYBIND11_DECLARE_HOLDER_TYPE does not work. :-(
 */
//PYBIND11_DECLARE_HOLDER_TYPE(T, SharedPtr<T>);
PYBIND11_NAMESPACE_BEGIN(PYBIND11_NAMESPACE)
namespace detail {
  template <typename T>
  class type_caster<SharedPtr<T>>
      : public copyable_holder_caster<T, SharedPtr<T>> {};
  template <typename T>
  struct is_holder_type<T, SharedPtr<T>> : std::true_type {};
}
PYBIND11_NAMESPACE_END(PYBIND11_NAMESPACE)
