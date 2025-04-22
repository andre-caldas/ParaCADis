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

#include <libparacadis/base/geometric_primitives/spheres.h>
#include <libparacadis/base/geometric_primitives/types.h>

#include <python_bindings/types.h>

namespace py = pybind11;
using namespace py::literals;

using namespace Naming;
using namespace Document;

void init_geo_spheres(py::module_& module)
{
  py::class_<SphereCenterRadius2, DocumentSurface,
             SharedPtr<SphereCenterRadius2>>(
      module, "SphereCenterRadius2",
      "A sphere determined by its center and squared radius.")
      .def(py::init(&SharedPtr<SphereCenterRadius2>::make_shared<Point&, Real&>),
           "center"_a, "radius2"_a,
           "The sphere is determined by its 'center' and squared radius ('radius2').")
      .def("__repr__",
           [](const SphereCenterRadius2&)
           { return "<SPHERECENTERRADIUS2... (put info here)>"; });


  py::class_<SphereCenterSurfacePoint, DocumentSurface,
             SharedPtr<SphereCenterSurfacePoint>>(
      module, "SphereCenterSurfacePoint", py::multiple_inheritance(),
      "A sphere determined by its center and one surface point.")
      .def(py::init(&SharedPtr<SphereCenterSurfacePoint>::make_shared<Point&, Point&>),
           "center"_a, "surface_point"_a,
           "The sphere is determined by its 'center'"
           " and a 'surface_point'.")
      .def("__repr__",
           [](const SphereCenterSurfacePoint&)
           { return "<SPHERESURFACEPOINT... (put info here)>"; });
}
