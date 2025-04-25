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

module geo_types;

#include <cmath>

// TODO: split into separate files.

namespace geo_types
{
  /*
   * Real
   */
  Real& Real::operator+=(Real a)
  { value += a.value; return *this; }

  Real& Real::operator-=(Real a)
  { value -= a.value; return *this; }

  Real& Real::operator*=(Real a)
  { value *= a.value; return *this; }

  Real& Real::operator/=(Real a)
  { value /= a.value; return *this; }

  Real operator+(Real a, Real b)
  {
    return a.value + b.value;
  }

  Real operator-(Real a, Real b)
  {
    return a.value - b.value;
  }

  Real operator*(Real a, Real b)
  {
    return a.value * b.value;
  }

  Real operator/(Real a, Real b)
  {
    return a.value / b.value;
  }

  float to_float(Real a)
  {
    return a.value;
  }

  Real sqrt(Real a)
  {
    return std::sqrt(a.value);
  }


  /*
   * Vector
   */
  Real Vector::squared_length() const
  {
    return x*x + y*y + z*z;
  }

  Vector& Vector::operator+=(const Vector& v)
  { x += v.x; y += v.y; z += v.z; return *this; }

  Vector& Vector::operator-=(const Vector& v)
  { x -= v.x; y -= v.y; z -= v.z; return *this; }

  Vector& Vector::operator*=(Real a)
  { x *= a; y *= a; z *= a; return *this; }

  Vector& Vector::operator/=(Real a)
  { x /= a; y /= a; z /= a; return *this; }

  Vector operator+(const Vector& v, const Vector& w)
  {
    return {v.x + w.x, v.y + w.y, v.z + w.z};
  }

  Vector operator-(const Vector& v, const Vector& w)
  {
    return {v.x - w.x, v.y - w.y, v.z - w.z};
  }

  Vector operator*(const Real a, const Vector& v)
  {
    return {v.x * a.value, v.y * a.value, v.z * a.value};
  }

  Vector operator/(const Vector& v, const Real a)
  {
    return {v.x / a.value, v.y / a.value, v.z / a.value};
  }


  Real determinant(const Vector& u, const Vector& v, const Vector& w)
  {
    float result = 0;
    result += u.x() * v.y() * w.z();
    result += u.y() * v.z() * w.x();
    result += u.z() * v.x() * w.y();
    result -= u.z() * v.y() * w.x();
    result -= u.x() * v.z() * w.y();
    result -= u.y() * v.x() * w.z();
    return result;
  }

  Real dot_product(const Vector& v, const Vector& w)
  {
    return v.x*w.x + v.y*w.y + v.z*w.z;
  }

  Vector cross_product(const Vector& v, const Vector& w)
  {
    return { v.y*w.z - v.z*w.y, v.z*w.x - v.x*w.z, v.x*w.y - v.y*w.x };
  }

  Point Vector::to_point() const
  {
    return {x, y, z};
  }


  /*
   * Point
   */
  Point operator+(const Point& p, const Vector& v)
  {
    return { p.x + v.x, p.y + v.y, p.z + v.z };
  }

  Vector operator-(const Point& p, const Point& q)
  {
    return { p.x - q.x, p.y - q.y, p.z - q.z };
  }

  Vector Point::to_vector() const
  {
    return {x, y, z};
  }
}
