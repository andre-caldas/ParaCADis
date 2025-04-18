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

#include <format>

namespace py = pybind11;
using namespace py::literals;

using namespace Naming;

void init_geometric_primitives_vectors(py::module_& module)
{
  /*
   * Vector.
   */
  py::class_<Vector, SharedPtr<Vector>>(
      module, "Vector",
      "A vector.")
      .def(py::init<>(
               []() -> SharedPtr<Vector>
               {return std::make_shared<Vector>(0,0,0);}),
           "Zero vector: (0, 0, 0)")
      .def(py::init<Real, Real, Real>(), "x"_a, "y"_a, "z"_a)
      .def(py::init(&new_from_vector<Vector>))
      .def(py::self + py::self)
      .def(py::self - py::self)
      .def("set",
           [](SharedPtr<Vector>& self, const Vector& other) -> SharedPtr<Vector>&
           {*self = other; return self;})
      .def_property_readonly("x",
           [](const Vector& self) -> SharedPtr<Real>
           {return std::make_shared<Real>(self.x());},
           "The 'x' coordinate.")
      .def_property_readonly("y",
           [](const Vector& self) -> SharedPtr<Real>
           {return std::make_shared<Real>(self.y());},
           "The 'y' coordinate.")
      .def_property_readonly("z",
           [](const Vector& self) -> SharedPtr<Real>
           {return std::make_shared<Real>(self.z());},
           "The 'z' coordinate.")
      .def("__repr__",
           [](const Vector& self)
           {
             return std::format("<VECTOR... ({}, {}, {})>",
             CGAL::to_double(self.x()),
             CGAL::to_double(self.y()),
             CGAL::to_double(self.z()));
           });
  py::implicitly_convertible<py::list, Vector>();

  /*
   * DeferenceableVector.
   */
  py::class_<DeferenceableVector, ExporterCommon,
             SharedPtr<DeferenceableVector>>(
      module, "DeferenceableVector", py::multiple_inheritance(),
      "A vector that can be handled as a geometric object."
      " It can be put inside containers and it exports its parameters.")
      .def(py::init(&SharedPtr<DeferenceableVector>::make_shared<>), "Zero vector")
      .def(py::init(&SharedPtr<DeferenceableVector>::make_shared<Real&, Real&, Real&>),
           "x"_a, "y"_a, "z"_a)
      .def(py::init(&SharedPtr<DeferenceableVector>::make_shared<const Vector&>))
      .def("__repr__",
           [](const DeferenceableVector&)
           { return "<DEFERENCEABLEVECTOR... (put info here)>"; });
  bind_reference_to<DeferenceableVector>(module, "Vector");
}
