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

#pragma once

#include <pybind11/pybind11.h>

#include "reference_to.h"

#include <base/expected_behaviour/SharedPtr.h>
#include <base/naming_scheme/PathToken.h>
#include <base/naming_scheme/ReferenceToObject.h>
#include <base/naming_scheme/ResultHolder.h>

#include <base/threads/locks/reader_locks.h>
#include <base/threads/locks/writer_locks.h>

namespace py = pybind11;
using namespace py::literals;

template<typename T>
py::class_<NamingScheme::ReferenceTo<T>, SharedPtr<NamingScheme::ReferenceTo<T>>>
bind_reference_to(py::module_& m, std::string type_name)
{
  // TODO: use string_view concatenation in c++26. :-)

  using ReferenceTo  = NamingScheme::ReferenceTo<T>;
  using ResultHolder = NamingScheme::ResultHolder<T>;
  using ReaderGate   = Threads::ReaderGateKeeper<ResultHolder>;
  using WriterGate   = Threads::WriterGateKeeper<ResultHolder>;

  std::string ref_to_name        = "ReferenceTo"  + type_name;
  std::string reader_gate_name   = "ReaderGateTo" + type_name;
  std::string writer_gate_name   = "WriterGateTo" + type_name;
  std::string result_holder_name = "ResultHolder" + type_name;

  /*
   * ReferenceTo:
   * A path so some object.
   */
  py::class_<ReferenceTo, SharedPtr<ReferenceTo>>
  ref_to(m, ref_to_name.c_str(),
         "A path that can be resolved and locked to access data.");
  ref_to.def(py::init<SharedPtr<ExporterBase>>());
  ref_to.def(py::init<SharedPtr<ExporterBase>, std::string>());
  ref_to.def(py::init<SharedPtr<ExporterBase>, std::string, std::string>());
  ref_to.def(py::init<SharedPtr<ExporterBase>, std::string, std::string, std::string>());
  // construct. from base + strings. from path.

  ref_to.def("resolve", &ReferenceTo::resolve,
  R"(Searches the path and resolves the reference.

  Resolving consists of querying objects according to a path.
  When the final object is found, a ResultHolder is returned.
  While the ResultHolder is held, the object is guaranteed to exist
  although its path might have changed in the meanwhile.

  However, having a ResultHolder is not enough to access the data.
  ParaCADis is highly multithreaded and accessing data that
  is shared among threads requires holding a "gate".

  Returns
  =======
  A ResultHolder that:
  1. Ensures the object does not get destructed while the ResultHolder
     is held.
  2. Can be used to acquire a ReaderGate or a WriterGate that can be
     used to access the object data.
)");
  ref_to.def("resolve_and_lock",
             [](SharedPtr<ReferenceTo> self){return WriterGate{self->resolve()};},
             "Locks for writing and provides a gate to access the object.");
  // append string.
  // get just the path.
  ref_to.def("__repr__",
             [type_name](const ReferenceTo& ref)
             { return "<REFERENCETO... (" + type_name + ")>"; });


  /*
   * ReaderGate:
   * A gate that allows reading data from the resolved reference.
   */
  py::class_<ReaderGate, SharedPtr<ReaderGate>>
  rgate(m, reader_gate_name.c_str(),
  R"(Locks the <" + type_name + "> for reading.

  A "gate" is necessary to access the resolved object.
  Many reader gates can access the same data simultaneously.

  Attention
  =========
  1. When the gate is constructed, it blocks waiting for the release
     of any exclusive gate (or lock) to the same object.
  2. This gate causes any process that requests write access
     to the referenced object to block.
     Therefore, one should not hold a gate for too long.
  3. Check out the "lock policy" to learn how to use locks and gates.
)");
  rgate.def(py::init<ResultHolder>(), "ResultHolder"_a,
  R"(Constructs a reader gate for the resolved object.)");

  rgate.def("access",
  [](SharedPtr<ReaderGate> gate) -> SharedPtr<T>
  {
    auto shared = gate->getHolder()._promiscuousGetShared();
    return shared;
  },
  R"(Accesses the referenced object

  Attention
  =========
  1. You are not supposed to save the object for later reference.
     Objects are only safe to be accessed while holding the gate.
  2. Python will not forbid you to write to the object using a reader
     gate. Of course, you are not supposed to do that. :-P
)");

  rgate.def("release", &ReaderGate::release,
  R"(Prematurelly releases the gate to avoid other tasks being blocked.

  When the gate is garbage collected and is destructed, the associated
  locks are automatically released. However to explicitelly releasing
  those locks you can call this method.
)");


  /*
   * WriterGate:
   * A gate that allows writing data to the resolved reference.
   */
  py::class_<WriterGate, SharedPtr<WriterGate>>
  wgate{m, writer_gate_name.c_str(),
  R"(Locks the <" + type_name + "> for writing.

  A "writer gate" is necessary to exclusively access the resolved
  object.

  Attention
  =========
  1. When the gate is constructed, it blocks waiting for the release
     of all gates and locks to the same object to be released.
  2. This gate causes any process that requests any kind of access
     to the referenced object to block.
     Therefore, one should not hold a gate for too long.
  3. Check out the "lock policy" to learn how to use locks and gates.
)"};
  wgate.def(py::init<ResultHolder>(), "ResultHolder"_a,
  R"(Constructs an exclusive (write) gate for the resolved object.)");

  wgate.def("access",
  [](SharedPtr<WriterGate> gate) -> SharedPtr<T>
  {
    auto shared = gate->getHolder()._promiscuousGetShared();
    return shared;
  },
  R"(Accesses the referenced object

  Attention
  =========
  You are not supposed to save the object for later reference.
  Objects are only safe to be accessed while holding the gate.
)");

  wgate.def("release", &WriterGate::release,
  R"(Prematurelly releases the gate to avoid other tasks being blocked.

  When the gate is garbage collected and is destructed, the associated
  locks are automatically released. However to explicitelly releasing
  those locks you can call this method.
)");


  /*
   * ResultHolder:
   * Holds an instance for the resolved object.
   */
  py::class_<ResultHolder, SharedPtr<ResultHolder>>
  holder{m, result_holder_name.c_str()};
  //holder.def(py::init<>());
  holder.def("rgate",
             [](ResultHolder& self) -> SharedPtr<ReaderGate>
             {return std::make_shared<ReaderGate>(self);},
             "Locks for reading and provides a gate to access the object.");
  holder.def("wgate",
             [](ResultHolder& self) -> SharedPtr<WriterGate>
             {return std::make_shared<WriterGate>(self);},
             "Locks for writing and provides a gate to access the object.");

  return ref_to;
}
