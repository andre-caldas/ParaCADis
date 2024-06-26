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

#include "types.h"

#include <pybind11/pybind11.h>

#include <base/document_tree/python_bindings/module.h>
#include <base/geometric_primitives/python_bindings/module.h>
#include <base/naming_scheme/python_bindings/module.h>
#include <base/expected_behaviour/SharedPtr.h>

PYBIND11_MODULE(paracadis, m) {
  m.doc() = "ParaCADis python interface library.";

  init_naming_scheme(m);
  init_geometric_primitives(m);
  init_document_tree(m);
}
