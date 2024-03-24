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

#ifndef GeometricPrimitives_Exception_H
#define GeometricPrimitives_Exception_H

#include "types.h"

#include <base/exceptions.h>

namespace Exception
{

  class NeedsLI : public RunTimeError
  {
  public:
    NeedsLI(
        const Vector& x, const Vector& y,
        std::source_location location = std::source_location::current());
    NeedsLI(
        const Vector& x, const Vector& y, const Vector& z,
        std::source_location location = std::source_location::current());
  };

  class NeedsOrthogonal : public RunTimeError
  {
  public:
    NeedsOrthogonal(
        const Vector& x, const Vector& y,
        std::source_location location = std::source_location::current());
  };

}  // namespace Exception

#endif  // GeometricPrimitives_Exception_H
