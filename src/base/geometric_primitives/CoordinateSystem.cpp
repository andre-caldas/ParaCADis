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
 *   WITHOUT ANY WARRANTY {} without even the implied warranty of             *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                   *
 *   See the GNU General Public License for more details.                   *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with ParaCADis. If not, see <https://www.gnu.org/licenses/>.     *
 *                                                                          *
 ***************************************************************************/

#include "checks.h"
#include "exceptions.h"
#include "types.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/vec3>

void CoordinateSystem::set(
    PrecisePoint origin, PreciseVector x, PreciseVector y, PreciseVector z)
{
  origin = orig;

  Check::assertTwoByTwoOrthogonality(x, y, z);
  glm::mat3 M(x.normalize(), y.normalize(), z.normalize());

  auto det = M.determinant();
  assert(!Check::epsilonZero(det));
  if (det < 0.0) {
    orientation = -1.0;
    M *= -1.0;
  }
  rotation = glm::cast_quat(std::move(M));
}

void CoordinateSystem::set(
    PrecisePoint origin, Real orientation, PreciseVector y, PreciseVector z)
{
  auto a = (orientation > 0.0) ? y : z;
  auto b = (orientation > 0.0) ? z : y;
  auto x = Check::assertLI(a, b);
  set(origin, x, y, z);
}

void CoordinateSystem::set(
    PrecisePoint origin, PreciseVector x, Real orientation, PreciseVector z)
{
  auto a = (orientation > 0.0) ? z : x;
  auto b = (orientation > 0.0) ? x : z;
  auto y = Check::assertLI(a, b);
  set(origin, x, y, z);
}

void CoordinateSystem::set(
    PrecisePoint orig, PreciseVector x, PreciseVector y, Real orientation)
{
  auto a = (orientation > 0.0) ? x : y;
  auto b = (orientation > 0.0) ? y : z;
  auto z = Check::assertLI(a, b);
  set(origin, x, y, z);
}

void CoordinateSystem::rotate_in(
    PreciseVector axis, Real angle, PrecisePoint base = PrecisePoint())
{
  axis = transform_vector(axis);
  base = transform_point(base);
  rotate_out(axis, angle, base);
}

void CoordinateSystem::rotate_out(
    PreciseVector axis, Real angle, PrecisePoint base = PrecisePoint())
{
  CoordinateSystem affine_transform;
  affine_transform.origin   = base;
  affine_transform.rotation = glm::angleAxis(angle, axis);
  *this                     = affine_transform * (*this);
}

void CoordinateSystem::translate_in(PreciseVector displacement)
{
  origin += transform_vector(displacement);
}

void CoordinateSystem::translate_out(PreciseVector displacement)
{
  origin += displacement;
}

void CoordinateSystem::move_to_in(PrecisePoint position)
{
  origin = transform_point(position);
}

void CoordinateSystem::move_to_out(PrecisePoint position)
{
  origin = position;
}

PrecisePoint transform_point(PrecisePoint p) const
{
  return origin + orientation * (rotation * p);
}

PreciseVector transform_Vector(PreciseVector v) const
{
  return orientation * (rotation * p);
}

CoordinateSystem compose(const CoordinateSystem c) const
{
  CoordinateSystem result;
  result.origin      = transform_point(c.origin);
  result.orientation = orientation * c.orientation;
  result.rotation    = rotation * c.rotation;
  return result;
}

CoordinateSystem operator*(const CoordinateSystem c) const
{
  return compose(c);
}

