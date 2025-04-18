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

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include "module.h"

#include <base/geometric_primitives/deferenceables.h>
#include <base/geometric_primitives/types.h>
#include <base/naming/python_bindings/reference_to.h>

#include <python_bindings/types.h>

namespace py = pybind11;
using namespace py::literals;

using namespace Naming;

void init_geometric_primitives_reals(py::module_& module)
{
  py::class_<Real, SharedPtr<Real>>(
      module, "Real",
      "Real number used in ParaCADis."
      " In the future it will be an algebraic structure (field)"
      " at least with square root.")
      .def(py::init<>(
           []() -> SharedPtr<Real>
           {return std::make_shared<Real>(0.);}),
           "Creates a real number equals to 0.")
      .def(py::init<double>(),
           "Creates a real number from a python number.")
      .def("set",
           [](SharedPtr<Real>& self, const Real& other) -> SharedPtr<Real>&
           {*self = other; return self;})
      .def("__float__",
           [](const Real& r){return CGAL::to_double(r);})
      .def("__iadd__",
           [](SharedPtr<Real>& self, const Real& other) -> SharedPtr<Real>&
           {*self += other; return self;})
      .def("__isub__",
           [](SharedPtr<Real>& self, const Real& other) -> SharedPtr<Real>&
           {*self -= other; return self;})
      .def("__imul__",
           [](SharedPtr<Real>& self, const Real& other) -> SharedPtr<Real>&
           {*self *= other; return self;})
      .def("__imatmul__",
           [](SharedPtr<Real>& self, const Real& /*other*/) -> SharedPtr<Real>&
           {py::set_error(PyExc_NotImplementedError, "Method not implemented"); return self;})
      .def("__itruediv__",
           [](SharedPtr<Real>& self, const Real& other) -> SharedPtr<Real>&
           {*self /= other; return self;})
      .def("__ifloordiv__",
           [](SharedPtr<Real>& self, const Real& /*other*/) -> SharedPtr<Real>&
           {py::set_error(PyExc_NotImplementedError, "Method not implemented"); return self;})
      .def("__imod__",
           [](SharedPtr<Real>& self, const Real& /*other*/) -> SharedPtr<Real>&
           {py::set_error(PyExc_NotImplementedError, "Method not implemented"); return self;})
      .def("__ipow__",
           [](SharedPtr<Real>& self, const Real& /*other*/) -> SharedPtr<Real>&
           {py::set_error(PyExc_NotImplementedError, "Method not implemented"); return self;})
      .def("__ilshift__",
           [](SharedPtr<Real>& self, const Real& /*other*/) -> SharedPtr<Real>&
           {py::set_error(PyExc_NotImplementedError, "Method not implemented"); return self;})
      .def("__irshift__",
           [](SharedPtr<Real>& self, const Real& /*other*/) -> SharedPtr<Real>&
           {py::set_error(PyExc_NotImplementedError, "Method not implemented"); return self;})
      .def("__iand__",
           [](SharedPtr<Real>& self, const Real& /*other*/) -> SharedPtr<Real>&
           {py::set_error(PyExc_NotImplementedError, "Method not implemented"); return self;})
      .def("__ixor__",
           [](SharedPtr<Real>& self, const Real& /*other*/) -> SharedPtr<Real>&
           {py::set_error(PyExc_NotImplementedError, "Method not implemented"); return self;})
      .def("__ior__",
           [](SharedPtr<Real>& self, const Real& /*other*/) -> SharedPtr<Real>&
           {py::set_error(PyExc_NotImplementedError, "Method not implemented"); return self;})
      .def(py::self + py::self)
      .def(py::self - py::self)
      .def(py::self * py::self)
      .def(py::self / py::self)
      .def("__repr__",
           [](const Real& self)
           {
             return std::format("<REAL... ({})>",
             CGAL::to_double(self));
           });
  py::implicitly_convertible<const int, Real>();
  py::implicitly_convertible<const double, Real>();
  bind_reference_to<Real>(module, "Real");
}
