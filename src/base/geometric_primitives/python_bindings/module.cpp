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

#include <nanobind/nanobind.h>
#include <nanobind/operators.h>

#include "module.h"

#include <base/geometric_primitives/CoordinateSystem.h>
#include <base/geometric_primitives/circles.h>
#include <base/geometric_primitives/deferenceables.h>
#include <base/geometric_primitives/lines.h>
#include <base/geometric_primitives/types.h>

namespace nb = nanobind;
using namespace nb::literals;

void init_geometric_primitives(nb::module_& parent_module,
                               nb::module_& naming_scheme)
{
  auto m = parent_module.def_submodule("geometric_primitives");
  m.doc() = "Basic geometric objects used in ParaCADis.";

  auto exporter_base = naming_scheme.attr("ExporterBase");

  /*
   * Real number.
   */
  nb::class_<Real>(
      m, "Real",
      "Real number used in ParaCADis."
      " In the future it will be an algebraic structure (field)"
      " at least with square root.")
      .def(nb::init_implicit<double>(),
           "Creates a real number from a double."
           " (attention: do not use double for calculations).")
      .def(nb::self + nb::self)
      .def(nb::self - nb::self)
      .def(nb::self * nb::self)
      .def(nb::self / nb::self)
      .def("__repr__",
           [](const Real&){ return "<REAL... (put info here)>"; });


  /*
   * Vector.
   */
  nb::class_<Vector>(
      m, "Vector",
      "A vector.")
      .def(nb::init<>(), "Zero vector")
      .def(nb::init<Real, Real, Real>(), "x"_a, "y"_a, "z"_a)
      .def(nb::self + nb::self)
      .def(nb::self - nb::self)
      .def("__repr__",
           [](const Vector&){ return "<VECTOR... (put info here)>"; });

  /*
   * DeferenceableVector.
   */
  nb::class_<DeferenceableVector>(
      m, "DeferenceableVector", exporter_base,
      "A vector that can be handled as a geometric object."
      " It can be put inside containers and it exports its parameters.")
      .def(nb::init<>(), "Zero vector")
      .def(nb::init<Real, Real, Real>(), "x"_a, "y"_a, "z"_a)
      .def(nb::init<const Vector&>())
      .def("__repr__",
           [](const Vector&){ return "<DEFERENCEABLEVECTOR... (put info here)>"; });

  /*
   * Point.
   */
  nb::class_<Point>(
      m, "Point",
      "A point.")
      .def(nb::init<>(), "Origin")
      .def(nb::init<Real, Real, Real>(), "x"_a, "y"_a, "z"_a)
      .def("__repr__",
           [](const Point&){ return "<POINT... (put info here)>"; });

  /*
   * DeferenceablePoint.
   */
  nb::class_<DeferenceablePoint>(
      m, "DeferenceablePoint", exporter_base,
      "A point that can be handled as a geometric object."
      " It can be put inside containers and it exports its parameters.")
      .def(nb::init<>(), "Origin")
      .def(nb::init<Real, Real, Real>(), "x"_a, "y"_a, "z"_a)
      .def(nb::init<const Point&>())
      .def("__repr__",
           [](const Point&){ return "<DEFERENCEABLEPOINT... (put info here)>"; });


  /*
   * Line2Points.
   */
  nb::class_<Line2Points>(
      m, "Line2Points", exporter_base,
      "An oriented straight line, half-line or segment specified by two points.")
      .def(nb::init<Point, Point, bool, bool>(),
           "start"_a, "end"_a,
           "is_bounded_start"_a = true,
           "is_bounded_end"_a = true,
           "The line is determined by the points 'start' and 'end',"
           " those points are also boundaries when 'is_bounded_start'"
           " or 'is_bounded_end' are set to True.")
      .def("__repr__",
           [](const Point&){ return "<LINE2POINTS... (put info here)>"; });

  /*
   * LinePointDirection.
   */
  nb::class_<LinePointDirection>(
      m, "LinePointDirection", exporter_base,
      "An oriented straight line, half-line or segment specified by"
      " a starting point and a vector.")
      .def(nb::init<Point, Vector, bool, bool>(),
           "start"_a, "direction"_a,
           "is_bounded_start"_a = true,
           "is_bounded_end"_a = true,
           "The line is determined by the points 'start' and 'start + direction',"
           " those points are also boundaries when 'is_bounded_start'"
           " or 'is_bounded_end' are set to True.")
      .def("__repr__",
           [](const Point&){ return "<LINEPOINTDIRECTION... (put info here)>"; });


  /*
   * CirclePointRadiusNormal.
   */
  nb::class_<CirclePointRadiusNormal>(
      m, "CirclePointRadiusNormal", exporter_base,
      "An oriented circle specified by a center point,"
      " the radius and a normal vector."
      " The circle orientation is determined by the right-hand rule"
      " applied to the normal vector"
      " (a right-hand holding the normal vector is such that the fingers"
      " point to the circle orientation).")
      .def(nb::init<Point, Real, Vector>(),
           "center"_a, "radius"_a, "normal"_a,
           "Creates the circle cetered at 'center', with radius 'radius'"
           " and orientation given the the 'normal' vector.")
      .def("__repr__",
           [](const Point&){ return "<CIRCLEPOINTRADIUSNORMAL... (put info here)>"; });

  /*
   * Circle3Points.
   */
  nb::class_<Circle3Points>(
      m, "Circle3Points", exporter_base,
      "An oriented circle specified by three points.")
      .def(nb::init<Point, Point, Point>(),
           "a"_a, "b"_a, "c"_a,
           "The circle passes by the given points."
           " The orientation is determined by the sequence 'a->b->c'.")
      .def("__repr__",
           [](const Point&){ return "<CIRCLE3POINTS... (put info here)>"; });

  /*
   * DeferenceableCoordinateSystem.
   */
  nb::class_<DeferenceableCoordinateSystem>(
      m, "DeferenceableCoordinateSystem", exporter_base,
      "A coordinate system that exports its parameters.")
      .def(nb::init<>(), "Identity coordinate system.")
      .def(nb::init<const Point&>(), "A translated coordinate system.")
      .def(nb::init<const Point&, const Vector&, const Vector&, const Vector&>(),
           "Translated coordinate system with axis."
           " The axis need to be orthonormal.")
      .def("__repr__",
           [](const Point&){ return "<DEF_COORDINATESYSTEM... (put info here)>"; });
}

#endif
