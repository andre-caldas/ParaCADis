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

#include <nanobind/nanobind.h>

#include "scene.h"

#include <base/expected_behaviour/SharedPtr.h>
#include <base/document_tree/DocumentTree.h>
#include <python_bindings/SharedPtr_type_caster.h>
#include <scene_graph/SceneRoot.h>

namespace nb = nanobind;
using namespace nb::literals;
using namespace DocumentTree;
using namespace SceneGraph;

namespace {
  SharedPtr<SceneRoot> new_scene()
  {
    auto result = std::make_shared<SceneRoot>();
    result->runQueue();
    return result;
  }
}

void init_scene(nb::module_& m)
{
  nb::class_<SceneRoot>(
      m, "Scene",
      "A scene graph with a message queue that keeps it updated.")
      .def(nb::new_(&new_scene),
           "Creates an empty scene.")
      .def("populate", &SceneRoot::populate, "document"_a,
           "Populates the scene with the contents of 'document'.")
      .def("__repr__",
           [](const SceneRoot& s){ return "<SCENE... (put info here)>"; });
}
