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

#include <base/document_tree/Container.h>
#include <base/document_tree/DocumentTree.h>
#include <base/naming_scheme/PathToken.h>
#include <python_bindings/types.h>

namespace py = pybind11;
using namespace py::literals;
using namespace Document;
using namespace NamingScheme;

void init_document_tree(py::module_& parent_module)
{
  auto m = parent_module.def_submodule("document");
  m.doc() = "Manages the nodes in a ParaCADis document structure.";

  py::class_<Container, ExporterBase, SharedPtr<Container>>(
      m, "Container", py::multiple_inheritance(),
      "A container with coordinate system to hold other objects or containers.")
      .def(py::init<>(),
           "Creates an empty container.")
#if 0
      .def("contains",
           py::overload_cast<Uuid::uuid_type>(&Container::contains), "uuid"_a,
           "Verifies if the container holds or not the corresponding element.")
      .def("get_element",
           py::overload_cast<Uuid::uuid_type>(&Container::contains), "uuid"_a,
           "Retrives the corresponding element.")
#endif
      .def("add_container", &Container::addContainer, "container"_a,
           "Adds a nested container.")
      .def("add_element", &Container::addElement, "element"_a,
           "Adds an element to the container.")
.def("add_element", [](const Container& self, std::shared_ptr<ExporterBase>){}, "element"_a,
     "Adds an element to the container.")
#if 0
      .def("remove_element", &Container::removeElement, xxxxx,
           "Removes the corresponding element from the container.")
      .def("move_element_to", &Container::moveElementTo, xxxxx,
           "Moves an element from one container to some other container.")
#endif
      .def("__repr__",
           [](const Container& c){ return "<CONTAINER... (put info here)>"; });


  py::class_<DocumentTree, Container, SharedPtr<DocumentTree>>(
      m, "Document", py::multiple_inheritance(),
      "A container to hold a full document.")
      .def(py::init<>(),
           "Creates an empty document.")
      .def("__repr__",
           [](const DocumentTree& d){ return "<DOCUMENT... (put info here)>"; });
}
