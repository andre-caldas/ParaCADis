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

#include "types.h"

#include <libparacadis/base/document_tree/python_bindings/module.h>
#include <libparacadis/base/geometric_primitives/python_bindings/module.h>
#include <libparacadis/base/naming/python_bindings/module.h>
#include <libparacadis/base/threads/python_bindings/module.h>

#include <libparacadis/scene_graph/python_bindings/rendering_scope.h>
#include <libparacadis/scene_graph/python_bindings/scene.h>

#include <misc/imgui/python_bindings/imgui_scope.h>

#include <libparacadis/base/expected_behaviour/SharedPtr.h>
#include <libparacadis/base/naming/Exporter.h>

using namespace Naming;

PYBIND11_MODULE(pyracadis, m) {
  m.doc() = "ParaCADis python interface library.";

  py::class_<ExporterCommon, SharedPtr<ExporterCommon>>
  exporter(m, "ExporterCommon", "Base class for types that export other types.");

  init_naming(m);
  init_geo(m);
  init_document_tree(m);
  init_threads(m);

  init_rendering_scope(m);
  init_scene(m);

  init_imgui(m);
}
