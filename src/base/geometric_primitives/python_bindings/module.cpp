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

#pragma once

#include "module.h"

#include "circles.h"
#include "coordinate_systems.h"
#include "lines.h"
#include "points.h"
#include "spheres.h"

namespace py = pybind11;
using namespace py::literals;

void init_geometric_primitives(py::module_& parent_module)
{
  auto m = parent_module.def_submodule("geometric_primitives");
  m.doc() = "Basic geometric objects used in ParaCADis.";

  init_geometric_primitives_points(m);
  init_geometric_primitives_coordinate_systems(m);

  init_geometric_primitives_lines(m);
  init_geometric_primitives_circles(m);
  init_geometric_primitives_spheres(m);
}
