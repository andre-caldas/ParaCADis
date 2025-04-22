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

#include "CoordinateSystem.h"

#include <cassert>

#include "../exceptions.h"
#include "../types.h"

CoordinateSystem::CoordinateSystem(Point origin) : _origin(std::move(origin))
{
}

CoordinateSystem::CoordinateSystem(Point origin, AxisOrder order, Vector v, Vector w)
    : _origin(origin)
{
  // x = normalized v.
  auto x = v / CGAL::sqrt(v.squared_length());
  getAxisVector(order, 0) = x;

  // z = normalized (v x w)
  auto z = CGAL::cross_product(v, w);
  z /= CGAL::sqrt(z.squared_length());
  getAxisVector(order, 2) = z;

  // y = (z x x)
  getAxisVector(order, 1) = CGAL::cross_product(z, x);
}

Vector& CoordinateSystem::getAxisVector(AxisOrder order, int j)
{
  switch(j) {
    case 0:
      switch(order) {
        case AxisOrder::XY:
        case AxisOrder::XZ:
          return bx;
        case AxisOrder::YX:
        case AxisOrder::YZ:
          return by;
        case AxisOrder::ZX:
        case AxisOrder::ZY:
          return bz;
        default:
          assert(false);
      }
    case 1:
      switch(order) {
        case AxisOrder::YX:
        case AxisOrder::ZX:
          return bx;
        case AxisOrder::XY:
        case AxisOrder::ZY:
          return by;
        case AxisOrder::XZ:
        case AxisOrder::YZ:
          return bz;
        default:
          assert(false);
      }
    case 2:
      switch(order) {
        case AxisOrder::YZ:
        case AxisOrder::ZY:
          return bx;
        case AxisOrder::XZ:
        case AxisOrder::ZX:
          return by;
        case AxisOrder::XY:
        case AxisOrder::YX:
          return bz;
        default:
          assert(false);
      }
    default:
      assert(false && "Axis Vector must be 0, 1 or 2.");
      return bx;
  }
}
