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

#include <base/geometric_primitives/lines.h>
#include <base/geometric_primitives/types.h>
#include <python_bindings/types.h>

namespace py = pybind11;
using namespace py::literals;

using namespace NamingScheme;

void init_geometric_primitives_lines(py::module_& module)
{
  /*
   * Line2Points.
   */
  py::class_<Line2Points, ExporterBase, SharedPtr<Line2Points>>(
      module, "Line2Points", py::multiple_inheritance(),
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
      module, "LinePointDirection", py::multiple_inheritance(),
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
}
