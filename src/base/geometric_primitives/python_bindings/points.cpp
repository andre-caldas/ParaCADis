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
#include <python_bindings/types.h>

namespace py = pybind11;
using namespace py::literals;

using namespace NamingScheme;

namespace {
  template<typename T, typename X = double>
  SharedPtr<T> new_from_vector(const std::vector<X>& v) {
    return std::make_shared<T>(
        Real((v.size() > 0)?v[0]:X(0)),
        Real((v.size() > 1)?v[1]:X(0)),
        Real((v.size() > 2)?v[2]:X(0))
    );
  }
}

void init_geometric_primitives_points(py::module_& module)
{
  /*
   * Real number.
   */
  py::class_<Real, SharedPtr<Real>>(
      module, "Real",
      "Real number used in ParaCADis."
      " In the future it will be an algebraic structure (field)"
      " at least with square root.")
      .def(py::init<double>(),
           "Creates a real number from a double."
           " (attention: do not use double for calculations).")
      .def(py::self + py::self)
      .def(py::self - py::self)
      .def(py::self * py::self)
      .def(py::self / py::self)
      .def("__repr__",
           [](const Real&){ return "<REAL... (put info here)>"; });
  py::implicitly_convertible<const int, Real>();
  py::implicitly_convertible<const double, Real>();

  /*
   * Vector.
   */
  py::class_<Vector, SharedPtr<Vector>>(
      module, "Vector",
      "A vector.")
      .def(py::init<>(), "Zero vector")
      .def(py::init<Real, Real, Real>(), "x"_a, "y"_a, "z"_a)
      .def(py::init(&new_from_vector<Vector>))
      .def(py::self + py::self)
      .def(py::self - py::self)
      .def("__repr__",
           [](const Vector&){ return "<VECTOR... (put info here)>"; });
  py::implicitly_convertible<py::list, Vector>();

  /*
   * DeferenceableVector.
   */
  py::class_<DeferenceableVector, ExporterBase,
             SharedPtr<DeferenceableVector>>(
      module, "DeferenceableVector", py::multiple_inheritance(),
      "A vector that can be handled as a geometric object."
      " It can be put inside containers and it exports its parameters.")
      .def(py::init<>(), "Zero vector")
      .def(py::init<Real, Real, Real>(), "x"_a, "y"_a, "z"_a)
      .def(py::init<const Vector&>())
      .def("__repr__",
           [](const DeferenceableVector&)
           { return "<DEFERENCEABLEVECTOR... (put info here)>"; });

  /*
   * Point.
   */
  py::class_<Point, SharedPtr<Point>>(
      module, "Point", "A point.")
      .def(py::init<>(), "Origin")
      .def(py::init<Real, Real, Real>(), "x"_a, "y"_a, "z"_a)
      .def(py::init(&new_from_vector<Point>))
      .def("__repr__",
           [](const Point&){ return "<POINT... (put info here)>"; });
  py::implicitly_convertible<py::list, Point>();

  /*
   * DeferenceablePoint.
   */
  py::class_<DeferenceablePoint, ExporterBase,
             SharedPtr<DeferenceablePoint>>(
      module, "DeferenceablePoint", py::multiple_inheritance(),
      "A point that can be handled as a geometric object."
      " It can be put inside containers and it exports its parameters.")
      .def(py::init<>(), "Origin")
      .def(py::init<Real, Real, Real>(), "x"_a, "y"_a, "z"_a)
      .def(py::init<const Point&>())
      .def("__repr__",
           [](const DeferenceablePoint&)
           { return "<DEFERENCEABLEPOINT... (put info here)>"; });
}
