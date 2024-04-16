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

#include "CoordinateSystem.h"

#include "checks.h"
#include "exceptions.h"
#include "types.h"

CoordinateSystem::CoordinateSystem(Point origin) : origin(std::move(origin))
{
}

CoordinateSystem::CoordinateSystem(Point origin, Vector x, Vector y, Vector z)
    : origin(std::move(origin))
    , bx(std::move(x))
    , by(std::move(y))
    , bz(std::move(z))
{
}


void CoordinateSystem::set(Point origin_, Vector x, Vector y, Vector z)
{
  Check::assertTwoByTwoOrthogonality(x, y, z);
  origin = std::move(origin_);
  bx     = std::move(x);
  by     = std::move(y);
  bz     = std::move(z);
}

void CoordinateSystem::set(Point origin, Real orientation, Vector y, Vector z)
{
  auto& a = (orientation > 0.0) ? y : z;
  auto& b = (orientation > 0.0) ? z : y;
  auto  x = Check::assertLI(a, b);
  set(origin, std::move(x), std::move(y), std::move(z));
}

void CoordinateSystem::set(Point origin, Vector x, Real orientation, Vector z)
{
  auto& a = (orientation > 0.0) ? z : x;
  auto& b = (orientation > 0.0) ? x : z;
  auto  y = Check::assertLI(a, b);
  set(origin, std::move(x), std::move(y), std::move(z));
}

void CoordinateSystem::set(Point origin, Vector x, Vector y, Real orientation)
{
  auto& a = (orientation > 0.0) ? x : y;
  auto& b = (orientation > 0.0) ? y : x;
  auto  z = Check::assertLI(a, b);
  set(origin, std::move(x), std::move(y), std::move(z));
}

void CoordinateSystem::translateIn(const Vector& displacement)
{
  origin = origin + transform(displacement);
}

void CoordinateSystem::translateOut(const Vector& displacement)
{
  origin = origin + displacement;
}

void CoordinateSystem::moveToIn(const Point& position)
{
  origin = transform(position);
}

void CoordinateSystem::moveToOut(Point position)
{
  origin = std::move(position);
}

Point CoordinateSystem::transform(const Point& p) const
{
  // TODO: make use of homogeneous coordinates.
  return origin + p.x() * bx + p.y() * by + p.z() * bz;
}

Vector CoordinateSystem::transform(const Vector& v) const
{
  // TODO: make use of homogeneous coordinates.
  return v.x() * bx + v.y() * by + v.z() * bz;
}

CoordinateSystem CoordinateSystem::compose(const CoordinateSystem& c) const
{
  Point  o = transform(c.origin);
  Vector x = transform(c.bx);
  Vector y = transform(c.by);
  Vector z = transform(c.bz);
  return CoordinateSystem{std::move(o), std::move(x), std::move(y), std::move(z)};
}


/*
 * DeferenceableCoordinateSystem.
 */
DeferenceableCoordinateSystem::DeferenceableCoordinateSystem(Point origin) noexcept
    : Exporter{std::move(origin)}
{
}

DeferenceableCoordinateSystem::DeferenceableCoordinateSystem(
    Point origin, Vector x, Vector y, Vector z) noexcept
    : Exporter{std::move(origin), std::move(x), std::move(y), std::move(z)}
{
}

CoordinateSystem DeferenceableCoordinateSystem::getCoordinateSystem() const noexcept
{
  Threads::ReaderGate gate{*this};
  return {gate->origin, gate->bx, gate->by, gate->bz};
}

std::unique_ptr<DeferenceableCoordinateSystem> DeferenceableCoordinateSystem::deepCopy() const
{
  Threads::ReaderGate gate{*this};
  return std::make_unique<DeferenceableCoordinateSystem>(
      gate->origin, gate->bx, gate->by, gate->bz);
}
