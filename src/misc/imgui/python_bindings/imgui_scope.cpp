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

#include "imgui_scope.h"

#include <exception>

#include <python_bindings/types.h>

#include <base/expected_behaviour/SharedPtr.h>
#include <base/geometric_primitives/power_cast.h>
#include <base/geometric_primitives/all_translators.h>
#include <base/naming_scheme/Exporter.h>

#include <misc/imgui/ImGuiScope.h>

#include <OGRE/Overlay/OgreImGuiOverlay.h>

namespace py = pybind11;
using namespace py::literals;

using namespace ParacadisImGui;
using namespace NamingScheme;

namespace {
  void create_geometry_dialog(ImGuiScope& self, SharedPtr<ExporterCommon> exporter)
  {
    GeometryCast::dispatch(
        std::move(exporter),
        [&self]<typename T>(SharedPtr<T> geo){
          auto lambda_draw = [](ImGuiScope::Translator<T>& /*translator*/){
            ImGui::Text("Testando...");
            return true;
          };
          self.addMutexHolder(std::move(geo), std::move(lambda_draw));
        });
  }
}

void init_imgui(py::module_& m)
{
  py::module_::import("Ogre.Bites");
  py::class_<ImGuiScope, SharedPtr<ImGuiScope>>(
    m, "ImGuiScope",
    "Manipulates Dear ImGui's elements.")
    .def(py::init<>(),
         "Creates a ImGui scope you need to add to the RenderingScope.")
    .def("create_geometry_dialog",
         create_geometry_dialog , "native_geometry"_a,
         "Creates a 'dialog' for a dataset.")
    .def("__repr__",
         [](const ImGuiScope&){ return "<ImGuiScope... (put info here)>"; });
}
