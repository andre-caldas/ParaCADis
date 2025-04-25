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

#include "../types.h"

/**
 * Stores and manages the placement of an orthogonal coordinate system.
 *
 * Vector coordinates need to be interpreted in some context.
 * To place a coordinate system `C` inside some other coordinate system `G`,
 * we need to specify how points in `C` are converted to points in `G`.
 * This is what CoordinateSystem does.
 *
 * In practice, however, the user does not care about how calculations are done.
 * The user does not care if we use quaternions, matrices or what.
 * For the user, this is basically a frame of reference.
 * The user wants to put things inside this coordinate system and have them
 * to appear on the right place.
 * So, I (André Caldas) refuse to call this a Rotation or a Quaternion. :-P
 *
 * By orthogonal, we mean rotation: det = 1 and orthonormal basis.
 * I suppose a mathematician would call it an element of SO(3).
 * By placement we mean we can choose where is the origin of our coordinate system.
 */
class CoordinateSystem
{
public:
  CoordinateSystem() = default;
  CoordinateSystem(Point origin);

  enum class AxisOrder {
    XY, YZ, ZX,
    YX, ZY, XZ,
  };

  /**
   * Constructs a coordinate system placed @a origin,
   * by setting the axis indicated by @a order.
   *
   * The vector v is normalized and made the first axis (according to order).
   * Then, w has its component parallel to v striped and it is normalizes.
   * The result becomes the second axis. The last axis is (v x w) normalized.
   */
  CoordinateSystem(Point origin, AxisOrder order, Vector v, Vector w);

  /**
   * Gets the vector that represents the n-th axis.
   *
   * @example
   * assert(getAxisVector(AxisOrder::XY, 0) == 0);
   * assert(getAxisVector(AxisOrder::XY, 1) == 1);
   * assert(getAxisVector(AxisOrder::YX, 0) == 1);
   * assert(getAxisVector(AxisOrder::YX, 1) == 0);
   * assert(getAxisVector(AxisOrder::YX, 2) == 2);
   * assert(getAxisVector(AxisOrder::ZX, 2) == 1);
   *
   * @todo make constexpr with modules.
   */
  Vector& getAxisVector(AxisOrder order, int n);

  const Point&  origin() const {return _origin;}
  const Vector& x_axis() const {return bx;}
  const Vector& y_axis() const {return by;}
  const Vector& z_axis() const {return bz;}

private:
  Point  _origin = {0,0,0};
  Vector bx      = {1,0,0};
  Vector by      = {0,1,0};
  Vector bz      = {0,0,1};
};
