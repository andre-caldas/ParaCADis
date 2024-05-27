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

#include "scene.h"

#include <base/expected_behaviour/SharedPtr.h>
#include <base/document_tree/DocumentTree.h>
#include <python_bindings/types.h>
#include <scene_graph/SceneRoot.h>

namespace py = pybind11;
using namespace py::literals;
using namespace Document;
using namespace SceneGraph;

namespace {
  SharedPtr<SceneRoot> new_scene()
  {
    auto result = std::make_shared<SceneRoot>();
    result->runQueue();
    return result;
  }
}

void init_scene(py::module_& m)
{
  py::class_<SceneRoot, SharedPtr<SceneRoot>>(
      m, "Scene",
      "A scene graph with a message queue that keeps it updated.")
      .def(py::init(&new_scene),
           "Creates an empty scene.")
      .def("populate",
           [](const SharedPtr<SceneRoot>& self, const SharedPtr<DocumentTree>& doc)
           {self->populate(self, doc);},
           "document"_a,
           "Populates the scene with the contents of 'document'.")
      .def("__repr__",
           [](const SceneRoot& s){ return "<SCENE... (put info here)>"; });
}
