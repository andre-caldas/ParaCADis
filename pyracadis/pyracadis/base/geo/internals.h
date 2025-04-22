// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2024-2025 André Caldas <andre.em.caldas@gmail.com>       *
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

#include <libparacadis/base/expected_behaviour/SharedPtr.h>

#include <pyracadis/types.h>

namespace py = pybind11;

void init_geo_reals(py::module_& module);
void init_geo_points(py::module_& module);
void init_geo_vectors(py::module_& module);

void init_geo_lines(py::module_& module);
void init_geo_circles(py::module_& module);
void init_geo_spheres(py::module_& module);

void init_geo_coordinate_systems(py::module_& module);

template<typename T, typename X = double>
SharedPtr<T> new_from_vector(const std::vector<X>& v) {
  return std::make_shared<T>(
      (v.size() > 0)?v[0]:X(0),
      (v.size() > 1)?v[1]:X(0),
      (v.size() > 2)?v[2]:X(0));
}
