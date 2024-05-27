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

#include "module.h"

#include <base/naming_scheme/PathToken.h>

namespace py = pybind11;

template<typename T>
py::class_<NamingScheme::ReferenceTo<T>, SharedPtr<NamingScheme::ReferenceTo<T>>>
bind_reference_to(py::module_& m, std::string type_name)
{
  // TODO: use string_view concatenation in c++26. :-)
  py::class_<NamingScheme::ReferenceTo<T>, SharedPtr<NamingScheme::ReferenceTo<T>>> result{m, "ReferenceTo" + type_name};
  result
      //.def(py::init<>())
      .def("resolve", &ReferenceTo<T>::resolve)
      .def("__repr__",
           [](const NamingScheme::ReferenceTo<T>& ref)
           { return "<REFERENCETO... (put info here)>"; });
  return result;
}
