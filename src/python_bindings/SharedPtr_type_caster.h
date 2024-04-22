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

#ifndef PY_SHARED_PTR_TYPE_CASTER_H
#define PY_SHARED_PTR_TYPE_CASTER_H

#include <nanobind/stl/shared_ptr.h>
#include <base/expected_behaviour/SharedPtr.h>

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

template <typename T>
struct type_caster<SharedPtr<T>>
    : type_caster<std::shared_ptr<T>>
{
  NB_TYPE_CASTER(SharedPtr<T>, type_caster<std::shared_ptr<T>>::Caster::Name)

  bool from_python(handle src, uint8_t flags, cleanup_list *cleanup) noexcept
  {
    if(!type_caster<std::shared_ptr<T>>::from_python(src, flags, cleanup))
    {
      return false;
    }
    // Can we use std::move()? Or ::value is accessed by other people?
    value = std::move(type_caster<std::shared_ptr<T>>::value);
    return true;
  }

  static handle from_cpp(
      const Value &value, rv_policy p, cleanup_list *cleanup) noexcept
  {
    return type_caster<std::shared_ptr<T>>::from_cpp(value.sliced(), p, cleanup);
  }
};

NAMESPACE_END(detail)
NAMESPACE_END(NB_NAMESPACE)

#endif
