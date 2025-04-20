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

#include <base/expected_behaviour/SharedPtr.h>
#include <base/geometric_primitives/DocumentGeometry.h>
#include <base/naming/Exporter.h>

#include <python_bindings/types.h>

namespace py = pybind11;
using namespace py::literals;

using namespace Naming;
using namespace Document;

void init_geo(py::module_& parent_module)
{
  auto module = parent_module.def_submodule("geo");
  module.doc() = "Basic geometric objects used in ParaCADis.";

  py::class_<DocumentGeometry, ExporterCommon, SharedPtr<DocumentGeometry>>(
      module, "Document",
      "Base class for geometries in the document tree.")
      .def("__repr__",
           [](const DocumentGeometry&){ return "<GEOMETRY... (put info here)>"; });

  py::class_<DocumentCurve, DocumentGeometry, SharedPtr<DocumentCurve>>(
      module, "Curve",
      "Base class for curves in the document tree.")
      .def("__repr__",
           [](const DocumentCurve&){ return "<CURVE... (put info here)>"; });

  py::class_<DocumentSurface, DocumentGeometry, SharedPtr<DocumentSurface>>(
      module, "Surface",
      "Base class for surfaces in the document tree.")
      .def("__repr__",
           [](const DocumentSurface&){ return "<SURFACE... (put info here)>"; });

  init_geo_reals(module);
  init_geo_points(module);
  init_geo_vectors(module);

  init_geo_lines(module);
  init_geo_circles(module);
  init_geo_spheres(module);

  init_geo_coordinate_systems(module);
}
