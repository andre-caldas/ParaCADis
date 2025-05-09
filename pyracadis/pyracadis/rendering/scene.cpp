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

#include <exception>

#include <libparacadis/base/expected_behaviour/SharedPtr.h>
#include <libparacadis/base/document_tree/DocumentTree.h>
#include <libparacadis/scene_graph/SceneRoot.h>

//#include <filament/...>

#include <Python.h>

#include <pyracadis/types.h>

namespace py = pybind11;
using namespace py::literals;

using namespace Document;
using namespace SceneGraph;

void init_scene(py::module_& module)
{
  py::class_<SceneRoot, SharedPtr<SceneRoot>>(
      module, "Scene",
      "A scene graph with a message queue that keeps it updated.")
      .def(py::init<void*>(),
           "Creates an empty scene.")
      .def("populate",
           [](SharedPtr<SceneRoot> self, std::shared_ptr<DocumentTree> doc)
           {self->populate(std::move(self), std::move(doc));},
           "document"_a,
           "Populates the scene with the contents of 'document'.")
      .def("__repr__",
           [](const SceneRoot&){ return "<SCENE... (put info here)>"; });
}
