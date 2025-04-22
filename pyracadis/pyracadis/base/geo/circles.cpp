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

#include "internals.h"

#include <libparacadis/base/geometric_primitives/circles.h>
#include <libparacadis/base/geometric_primitives/types.h>

#include <python_bindings/types.h>

namespace py = pybind11;
using namespace py::literals;

using namespace Naming;
using namespace Document;

void init_geo_circles(py::module_& module)
{
  /*
   * CirclePointRadius2Normal.
   */
  py::class_<CirclePointRadius2Normal, DocumentCurve,
             SharedPtr<CirclePointRadius2Normal>>(
      module, "CirclePointRadius2Normal", py::multiple_inheritance(),
      "An oriented circle specified by a center point,"
      " the squared radius and a normal vector."
      " The circle orientation is determined by the right-hand rule"
      " applied to the normal vector"
      " (a right-hand holding the normal vector is such that the fingers"
      " point to the circle orientation).")
      .def(py::init(&SharedPtr<CirclePointRadius2Normal>::make_shared<Point&, Real&, Vector&>),
           "center"_a, "radius2"_a, "normal"_a,
           "Creates the circle cetered at 'center', with squared radius 'radius2'"
           " and orientation given the the 'normal' vector.")
      .def("__repr__",
           [](const CirclePointRadius2Normal&)
           { return "<CIRCLEPOINTRADIUSNORMAL... (put info here)>"; });

  /*
   * Circle3Points.
   */
  py::class_<Circle3Points, DocumentCurve,
             SharedPtr<Circle3Points>>(
      module, "Circle3Points", py::multiple_inheritance(),
      "An oriented circle specified by three points.")
      .def(py::init(&SharedPtr<Circle3Points>::make_shared<Point&, Point&, Point&>),
           "a"_a, "b"_a, "c"_a,
           "The circle passes by the given points."
           " The orientation is determined by the sequence 'a->b->c'.")
      .def("__repr__",
           [](const Circle3Points&)
           { return "<CIRCLE3POINTS... (put info here)>"; });
}
