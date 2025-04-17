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

#include "module.h"

#include <base/naming_scheme/Exporter.h>
#include <base/naming_scheme/PathToken.h>

#include <python_bindings/types.h>

namespace py = pybind11;
using namespace py::literals;
using namespace NamingScheme;

py::module_ init_naming_scheme(py::module_& parent_module)
{
  auto m = parent_module.def_submodule("naming_scheme");
  m.doc() = "Implements object access through a name/path mechanism.";

  py::class_<PathToken, SharedPtr<PathToken>>
  path(m, "PathToken",
       "A token that composes a path to an object."
       " Usually it is a name or a path_token returned by some method.");
  path.def(py::init<std::string>());
  path.def("__repr__",
           [](const PathToken&){ return "<PATHTOKEN... (put info here)>"; });

  py::class_<ExporterCommon, SharedPtr<ExporterCommon>>
  exporter(m, "ExporterCommon", "Base class for types that export other types.");

  return m;
}
