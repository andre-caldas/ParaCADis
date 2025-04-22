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

#include "module.h"

#include <libparacadis/base/geometric_primitives/deferenceables.h>
#include <libparacadis/base/geometric_primitives/types.h>
#include <libparacadis/base/naming/python_bindings/reference_to.h>

#include <format>

#include <python_bindings/types.h>
#include <pybind11/stl.h>  // We need to convert [1,2,3] to Point.

namespace py = pybind11;
using namespace py::literals;

using namespace Naming;

void init_geo_points(py::module_& module)
{
  /*
   * Point.
   */
  py::class_<Point, SharedPtr<Point>>(
      module, "Point", "A point.")
      .def(py::init<>(
               []() -> SharedPtr<Point>
               {return std::make_shared<Point>(0,0,0);}),
           "The origin: (0, 0, 0).")
      .def(py::init<Real, Real, Real>(), "x"_a, "y"_a, "z"_a)
      .def(py::init(&new_from_vector<Point>))
      .def("set",
           [](SharedPtr<Point>& self, const Point& other) -> SharedPtr<Point>&
           {*self = other; return self;})
      .def_property_readonly("x",
           [](const Point& self) -> SharedPtr<Real>
           {return std::make_shared<Real>(self.x());},
           "The 'x' coordinate.")
      .def_property_readonly("y",
           [](const Point& self) -> SharedPtr<Real>
           {return std::make_shared<Real>(self.y());},
           "The 'y' coordinate.")
      .def_property_readonly("z",
           [](const Point& self) -> SharedPtr<Real>
           {return std::make_shared<Real>(self.z());},
           "The 'z' coordinate.")
      .def("__repr__",
           [](const Point& self)
           {
             return std::format("<POINT... ({}, {}, {})>",
             CGAL::to_double(self.x()),
             CGAL::to_double(self.y()),
             CGAL::to_double(self.z()));
           });
  py::implicitly_convertible<py::list, Point>();

  /*
   * DeferenceablePoint.
   */
  py::class_<DeferenceablePoint, ExporterCommon,
             SharedPtr<DeferenceablePoint>>(
      module, "DeferenceablePoint", py::multiple_inheritance(),
      "A point that can be handled as a geometric object."
      " It can be put inside containers and it exports its parameters.")
      .def(py::init(&SharedPtr<DeferenceablePoint>::make_shared<>), "Origin")
      .def(py::init(&SharedPtr<DeferenceablePoint>::make_shared<Real&, Real&, Real&>),
           "x"_a, "y"_a, "z"_a)
      .def(py::init(&SharedPtr<DeferenceablePoint>::make_shared<const Point&>))
      .def("__repr__",
           [](const DeferenceablePoint&)
           { return "<DEFERENCEABLEPOINT... (put info here)>"; });
  bind_reference_to<DeferenceablePoint>(module, "Point");
}
