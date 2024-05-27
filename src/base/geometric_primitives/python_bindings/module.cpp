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

#ifndef GeometricPrimitives_PY_module_h
#define GeometricPrimitives_PY_module_h

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include "module.h"

#include <base/geometric_primitives/CoordinateSystem.h>
#include <base/geometric_primitives/circles.h>
#include <base/geometric_primitives/deferenceables.h>
#include <base/geometric_primitives/lines.h>
#include <base/geometric_primitives/types.h>
#include <python_bindings/types.h>

namespace py = pybind11;
using namespace py::literals;

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

void init_geometric_primitives(py::module_& parent_module)
{
  auto m = parent_module.def_submodule("geometric_primitives");
  m.doc() = "Basic geometric objects used in ParaCADis.";

  /*
   * Real number.
   */
  py::class_<Real, SharedPtr<Real>>(
      m, "Real",
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
      m, "Vector",
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
      m, "DeferenceableVector", py::multiple_inheritance(),
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
      m, "Point",
      "A point.")
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
      m, "DeferenceablePoint", py::multiple_inheritance(),
      "A point that can be handled as a geometric object."
      " It can be put inside containers and it exports its parameters.")
      .def(py::init<>(), "Origin")
      .def(py::init<Real, Real, Real>(), "x"_a, "y"_a, "z"_a)
      .def(py::init<const Point&>())
      .def("__repr__",
           [](const DeferenceablePoint&)
           { return "<DEFERENCEABLEPOINT... (put info here)>"; });


  /*
   * Line2Points.
   */
  py::class_<Line2Points, ExporterBase, SharedPtr<Line2Points>>(
      m, "Line2Points", py::multiple_inheritance(),
      "An oriented straight line, half-line or segment specified by two points.")
      .def(py::init<Point, Point, bool, bool>(),
           "start"_a, "end"_a,
           "is_bounded_start"_a = true,
           "is_bounded_end"_a = true,
           "The line is determined by the points 'start' and 'end',"
           " those points are also boundaries when 'is_bounded_start'"
           " or 'is_bounded_end' are set to True.")
      .def("__repr__",
           [](const Line2Points&)
           { return "<LINE2POINTS... (put info here)>"; });

  /*
   * LinePointDirection.
   */
  py::class_<LinePointDirection, ExporterBase,
             SharedPtr<LinePointDirection>>(
      m, "LinePointDirection", py::multiple_inheritance(),
      "An oriented straight line, half-line or segment specified by"
      " a starting point and a vector.")
      .def(py::init<Point, Vector, bool, bool>(),
           "start"_a, "direction"_a,
           "is_bounded_start"_a = true,
           "is_bounded_end"_a = true,
           "The line is determined by the points 'start' and 'start + direction',"
           " those points are also boundaries when 'is_bounded_start'"
           " or 'is_bounded_end' are set to True.")
      .def("__repr__",
           [](const LinePointDirection&)
           { return "<LINEPOINTDIRECTION... (put info here)>"; });


  /*
   * CirclePointRadius2Normal.
   */
  py::class_<CirclePointRadius2Normal, ExporterBase,
             SharedPtr<CirclePointRadius2Normal>>(
      m, "CirclePointRadius2Normal", py::multiple_inheritance(),
      "An oriented circle specified by a center point,"
      " the squared radius and a normal vector."
      " The circle orientation is determined by the right-hand rule"
      " applied to the normal vector"
      " (a right-hand holding the normal vector is such that the fingers"
      " point to the circle orientation).")
      .def(py::init<Point, Real, Vector>(),
           "center"_a, "radius2"_a, "normal"_a,
           "Creates the circle cetered at 'center', with squared radius 'radius2'"
           " and orientation given the the 'normal' vector.")
      .def("__repr__",
           [](const CirclePointRadius2Normal&)
           { return "<CIRCLEPOINTRADIUSNORMAL... (put info here)>"; });

  /*
   * Circle3Points.
   */
  py::class_<Circle3Points, SharedPtr<Circle3Points>>(
      m, "Circle3Points", py::multiple_inheritance(),
      "An oriented circle specified by three points.")
      .def(py::init<Point, Point, Point>(),
           "a"_a, "b"_a, "c"_a,
           "The circle passes by the given points."
           " The orientation is determined by the sequence 'a->b->c'.")
      .def("__repr__",
           [](const Circle3Points&)
           { return "<CIRCLE3POINTS... (put info here)>"; });

  /*
   * DeferenceableCoordinateSystem.
   */
  py::class_<DeferenceableCoordinateSystem, ExporterBase,
             SharedPtr<DeferenceableCoordinateSystem>>(
      m, "DeferenceableCoordinateSystem", py::multiple_inheritance(),
      "A coordinate system that exports its parameters.")
      .def(py::init<>(), "Identity coordinate system.")
      .def(py::init<const Point&>(), "A translated coordinate system.")
      .def(py::init<const Point&, const Vector&, const Vector&, const Vector&>(),
           "Translated coordinate system with axis."
           " The axis need to be orthonormal.")
      .def("__repr__",
           [](const DeferenceableCoordinateSystem&)
           { return "<DEF_COORDINATESYSTEM... (put info here)>"; });
}

#endif
