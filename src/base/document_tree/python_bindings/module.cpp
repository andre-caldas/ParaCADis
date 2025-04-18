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
#include <base/naming/PathToken.h>
#include <python_bindings/types.h>

namespace py = pybind11;
using namespace py::literals;
using namespace Document;
using namespace Naming;

void init_document_tree(py::module_& parent_module)
{
  auto m = parent_module.def_submodule("document");
  m.doc() = "Manages the nodes in a ParaCADis document structure.";

  py::class_<Container, ExporterCommon, SharedPtr<Container>> cont(
      m, "Container", py::multiple_inheritance(),
      "A container with coordinate system to hold other objects or containers.");
  cont.def(py::init<>(), "Creates an empty container.");
#if 0
  cont.def("contains",
           py::overload_cast<Uuid::uuid_type>(&Container::contains), "uuid"_a,
           "Verifies if the container holds or not the corresponding element.");
  cont.def("get_element",
           py::overload_cast<Uuid::uuid_type>(&Container::contains), "uuid"_a,
           "Retrives the corresponding element.");
#endif
  cont.def("add_container", &Container::addContainer, "container"_a,
           "Adds a nested container.");
  cont.def("add_element", &Container::addElement, "element"_a,
           "Adds an element to the container.");
#if 0
  cont.def("remove_element", &Container::removeElement, xxxxx,
           "Removes the corresponding element from the container.");
  cont.def("move_element_to", &Container::moveElementTo, xxxxx,
           "Moves an element from one container to some other container.");
#endif
  cont.def("set_coordinates", &Container::setCoordinates, "coordinate_system"_a,
           "Sets a deferenceable coordinate systema as the coordinate system for this container.");
  cont.def("__repr__",
           [](const Container&){ return "<CONTAINER... (put info here)>"; });


  py::class_<DocumentTree, Container, SharedPtr<DocumentTree>> tree(
      m, "Document", py::multiple_inheritance(),
      "A container to hold a full document.");
  tree.def(py::init<>(), "Creates an empty document.");
  tree.def("__repr__",
           [](const DocumentTree&){ return "<DOCUMENT... (put info here)>"; });
}
