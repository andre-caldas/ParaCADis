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

#include "scopes.h"

#include <base/threads/dedicated_thread_scope/DedicatedThreadScope.h>
#include <base/threads/dedicated_thread_scope/ScopeOfScopes.h>

#include <python_bindings/types.h>

namespace py = pybind11;
using namespace py::literals;
using namespace Threads;

void init_thread_scope(py::module_& m)
{
  /*
   * This only declares the class.
   * Since executing python code involves locking the GIL,
   * and since the dedicated thread is not supposed to block,
   * this class will not be instantiable in python.
   */
  py::class_<DedicatedThreadScopeBase, SharedPtr<DedicatedThreadScopeBase>>(
      m, "DedicatedThreadScope",
      "Safely executes instructions in a dedicated thread."
      " This object cannot be instantiated in python"
      " and cannot be extended in python.")
    .def("__repr__",
         [](const DedicatedThreadScopeBase&){ return "<DEDICATEDTHREADSCOPE>"; });

  py::class_<ScopeOfScopes, DedicatedThreadScopeBase, SharedPtr<ScopeOfScopes>>(
      m, "A scope that is an array of scopes.",
      "Safely adds new scopes.")
    .def("addScope", &ScopeOfScopes::addScope, "scope"_a,
           "Adds a scope that will be automatically removed when it is garbage collected.")
    .def("addScopeKeepAlive", &ScopeOfScopes::addScopeKeepAlive, "scope"_a,
           "Adds a scope that will never be removed.")
    .def("__repr__",
         [](const ScopeOfScopes&){ return "<SCOPEOFSCOPES>"; });
}
