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

#ifndef NamingScheme_PY_module_h
#define NamingScheme_PY_module_h

#include <nanobind/nanobind.h>

#include <base/naming_scheme/ReferenceToObject.h>

namespace nb = nanobind;

NB_MAKE_OPAQUE(Uuid::uuid_type);

nb::module_ init_naming_scheme(nb::module_& parent_module);

template<typename T>
nb::class_<NamingScheme::ReferenceTo<T>>
bind_reference_to(nb::module_& m, std::string type_name);

#include"module_impl.h"

#endif
