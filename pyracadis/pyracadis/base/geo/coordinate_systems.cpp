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

#include <libparacadis/base/geometric_primitives/coordinate_system/DeferenceableCoordinateSystem.h>
#include <libparacadis/base/geometric_primitives/coordinate_system/DeferenceableCoordinatePoints.h>
#include <libparacadis/base/geometric_primitives/types.h>

#include <pyracadis/types.h>

namespace py = pybind11;
using namespace py::literals;

using namespace Naming;

void init_geo_coordinate_systems(py::module_& module)
{
  /*
   * Dummy base class: DeferenceableCoordinates.
   */
  py::class_<DeferenceableCoordinates, ExporterCommon,
             SharedPtr<DeferenceableCoordinates>>(
      module, "DeferenceableCoordinates", py::multiple_inheritance(),
      "Base class for coordinate systems that exports its parameters.")
      .def("__repr__",
           [](const DeferenceableCoordinates&)
           { return "<DEF_COORDINATESYSTEMS - BASE... (put info here)>"; });


  /*
   * DeferenceableCoordinateSystem.
   */
  py::class_<DeferenceableCoordinateSystemXY, DeferenceableCoordinates,
             SharedPtr<DeferenceableCoordinateSystemXY>>(
      module, "DeferenceableCoordinateSystemXY", py::multiple_inheritance(),
      "A coordinate system that exports its parameters.")
      .def(py::init(&SharedPtr<DeferenceableCoordinateSystemXY>::make_shared<>),
           "Identity coordinate system.")
      .def(py::init(&SharedPtr<DeferenceableCoordinateSystemXY>::make_shared<const Point&>),
           "A translated coordinate system.")
      .def(py::init(&SharedPtr<DeferenceableCoordinateSystemXY>::make_shared<
                    const Point&, const Vector&, const Vector&>),
           "Translated coordinate system with axis."
           " The axis need to be orthonormal.")
      .def("__repr__",
           [](const DeferenceableCoordinateSystemXY&)
           { return "<DEF_COORDINATESYSTEM - XY... (put info here)>"; });


  /*
   * DeferenceableCoordinatePoints.
   */
  py::class_<DeferenceableCoordinatePointsXY, DeferenceableCoordinates,
             SharedPtr<DeferenceableCoordinatePointsXY>>(
      module, "DeferenceableCoordinatePointsXY", py::multiple_inheritance(),
      "A coordinate system that exports its parameters.")
      .def(py::init(&SharedPtr<DeferenceableCoordinatePointsXY>::make_shared<>),
           "Identity coordinate system.")
      .def(py::init(&SharedPtr<DeferenceableCoordinatePointsXY>::make_shared<const Point&>),
           "A translated coordinate system.")
      .def(py::init(&SharedPtr<DeferenceableCoordinatePointsXY>::make_shared<
                    const Point&, const Point&, const Point&>),
           "Translated coordinate system with axis."
           " The axis need to be orthonormal.")
      .def("__repr__",
           [](const DeferenceableCoordinatePointsXY&)
           { return "<DEF_COORDINATEPOINTS - XY... (put info here)>"; });
}
