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

#include "module.h"

#include <base/document_tree/Container.h>
#include <base/document_tree/DocumentTree.h>
#include <base/naming_scheme/PathToken.h>
#include <python_bindings/SharedPtr_type_caster.h>

namespace nb = nanobind;
using namespace nb::literals;
using namespace Document;
using namespace NamingScheme;

void init_document_tree(nb::module_& parent_module)
{
  auto m = parent_module.def_submodule("document");
  m.doc() = "Manages the nodes in a ParaCADis document structure.";
  nb::class_<Container>(
      m, "Container",
      "A container with coordinate system to hold other objects or containers.")
      .def(nb::init<>(),
           "Creates an empty container.")
#if 0
      .def("contains",
           nb::overload_cast<Uuid::uuid_type>(&Container::contains), "uuid"_a,
           "Verifies if the container holds or not the corresponding element.")
      .def("get_element",
           nb::overload_cast<Uuid::uuid_type>(&Container::contains), "uuid"_a,
           "Retrives the corresponding element.")
#endif
      .def("add_element", &Container::addElement, "element"_a,
           "Adds an element to the container.")
#if 0
      .def("remove_element", &Container::removeElement, xxxxx,
           "Removes the corresponding element from the container.")
      .def("move_element_to", &Container::moveElementTo, xxxxx,
           "Moves an element from one container to some other container.")
#endif
      .def("__repr__",
           [](const Container& c){ return "<CONTAINER... (put info here)>"; });


  nb::class_<DocumentTree, Container>(
      m, "Document",
      "A container to hold a full document.")
      .def(nb::init<>(),
           "Creates an empty document.")
      .def("__repr__",
           [](const DocumentTree& d){ return "<DOCUMENT... (put info here)>"; });
}
