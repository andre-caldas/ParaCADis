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

CoordinateSystem::CoordinateSystem()
    : transform(IDENTITY)
{
}

void CoordinateSystem::set(Point origin, Vector x, Vector y, Vector z)
{
  Check::assertTwoByTwoOrthogonal(x, y, z);

  auto& o = origin;
  auto  a = y.hw() * z.hw() * o.hw();
  auto  b = x.hw() * z.hw() * o.hw();
  auto  c = x.hw() * y.hw() * o.hw();
  auto  d = x.hw() * y.hw() * z.hw();
  // clang-format off
  transform = AffineTransform(
        a * x.hx(), b * y.hx(), c * z.hx(), d * o.hx(),
        a * x.hy(), b * y.hy(), c * z.hy(), d * o.hy(),
        a * x.hz(), b * y.hz(), c * z.hz(), d * o.hz(),
        /*             zero              */ d * o.hw());
  // clang-format on
}

void CoordinateSystem::set(Point origin, Real orientation, Vector y, Vector z)
{
  auto a = (orientation > 0.0) ? y : z;
  auto b = (orientation > 0.0) ? z : y;
  auto x = Check::assertLI(a, b);
  set(origin, x, y, z);
}

void CoordinateSystem::set(Point origin, Vector x, Real orientation, Vector z)
{
  auto a = (orientation > 0.0) ? z : x;
  auto b = (orientation > 0.0) ? x : z;
  auto y = Check::assertLI(a, b);
  set(origin, x, y, z);
}

void CoordinateSystem::set(Point orig, Vector x, Vector y, Real orientation)
{
  auto a = (orientation > 0.0) ? x : y;
  auto b = (orientation > 0.0) ? y : z;
  auto z = Check::assertLI(a, b);
  set(origin, x, y, z);
}

void CoordinateSystem::translate_in(Vector displacement)
{
  translate_out(transform_vector(displacement));
}

void CoordinateSystem::translate_out(Vector displacement)
{
  move_to_out(origin + displacement);
}

void CoordinateSystem::move_to_in(Point position)
{
  move_to_out(transform_point(position));
}

void CoordinateSystem::move_to_out(Point position)
{
  auto& t = *transform;
  auto  v = p.hw();
  auto  w = t.hm(3,3);
  // clang-format off
  transform = AffineTransform(
        v * t.hm(0,0), v * t.hm(1,0), v * t.hm(2,2), w * position.hx(),
        v * t.hm(1,0), v * t.hm(1,1), v * t.hm(2,2), w * position.hy(),
        v * t.hm(2,0), v * t.hm(1,2), v * t.hm(2,2), w * position.hz(),
        /*                   zero                 */ w * v);
  // clang-format on
}

Point transform_point(Point p) const
{
  return transform->transform(p);
}

Vector transform_Vector(Vector v) const
{
  return transform->transform(v);
}

CoordinateSystem compose(const CoordinateSystem c) const
{
  return *transform * *c.transform;
}

CoordinateSystem operator*(const CoordinateSystem c) const
{
  return compose(c);
}

