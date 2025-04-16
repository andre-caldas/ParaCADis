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

#include <exception>

#include <python_bindings/types.h>

#include <base/expected_behaviour/SharedPtr.h>
#include <base/document_tree/DocumentTree.h>
#include <scene_graph/SceneRoot.h>

#include <OGRE/OgreRoot.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/Bites/OgreApplicationContext.h>
#include <OGRE/Bites/OgreInput.h>

#include <Python.h>

namespace py = pybind11;
using namespace py::literals;
using namespace Document;
using namespace SceneGraph;

namespace {
  /**
   * A hack to extract an object form the SWIG wrapper.
   * Probably introduces security issues.
   */
  template<typename T>
  T* extract_from_swig(py::handle swig_wrapper)
  {
    typedef struct
    {
      PyObject_HEAD
      void* ptr;
    } SwigPyObject;

    PyObject* pySwigThis = PyObject_GetAttrString(swig_wrapper.ptr(), "this");
    if(!pySwigThis) {
      throw std::runtime_error("Passed object is not SWIG (I think!).");
    }
    auto* swig = reinterpret_cast<SwigPyObject*>(pySwigThis);
    return reinterpret_cast<T*>(swig->ptr);
  }

  SharedPtr<SceneRoot> new_scene(py::handle py_scn_mgr)
  {
    auto* scene_manager = extract_from_swig<Ogre::SceneManager>(py_scn_mgr);
    auto result = std::make_shared<SceneRoot>(*scene_manager);
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
           "Creates an empty scene and associates it to an OGRE SceneManager.")
      .def("populate",
           [](SharedPtr<SceneRoot> self, std::shared_ptr<DocumentTree> doc)
           {self->populate(std::move(self), std::move(doc));},
           "document"_a,
           "Populates the scene with the contents of 'document'.")
      .def("__repr__",
           [](const SceneRoot&){ return "<SCENE... (put info here)>"; });
}
