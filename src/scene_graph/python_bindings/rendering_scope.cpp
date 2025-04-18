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

#include "rendering_scope.h"

#include <exception>

#include <python_bindings/types.h>

#include <scene_graph/RenderingScope.h>

namespace py = pybind11;
using namespace py::literals;
using namespace Threads;
using namespace SceneGraph;

void init_rendering_scope(py::module_& m)
{
  py::class_<RenderingScope, ScopeOfScopes, SharedPtr<RenderingScope>>(
      m, "RenderingScope",
      "The rendering scope is a ScopeOfScopes registered to be executed"
      "\nby the rendering thread."
      "\n"
      "\nScopes execute commands sent by other threads."
      "\nIn the case of the RenderingScope, as a ScopeOfScopes,"
      "\nthose commands are for adding sub-scopes."
      "\nFor example, you can add a ImGuiScope. Then, you can send GUI elements"
      "\nfor this scope to execute Dear ImGui commands to render those GUI elements."
      "\n"
      "\nThis object is automatically instantiated by the Scene object."
      "\nYou (probably) should not instantiate it yourself.")
      .def("__repr__",
           [](const RenderingScope&){ return "<RENDERING SCOPE... (put info here)>"; });
}
