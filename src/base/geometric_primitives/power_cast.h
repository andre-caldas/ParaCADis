// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2025 André Caldas <andre.em.caldas@gmail.com>            *
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

#include "deferenceables.h"
#include "lines.h"
#include "circles.h"
#include "spheres.h"

#include <base/type_traits/PowerCast.h>

/**
 * Used to "down-cast" ExporterCommon and execute
 * the propper method, according to the derived type.
 */
using GeometryCast = TypeTraits::PowerCast<
  Naming::ExporterCommon,
  DeferenceablePoint, DeferenceableVector,
  Line2Points, LinePointDirection,
  CirclePointRadius2Normal, Circle3Points,
  SphereCenterRadius2, SphereCenterSurfacePoint
>;
