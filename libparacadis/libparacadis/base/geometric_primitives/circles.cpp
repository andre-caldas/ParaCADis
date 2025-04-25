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

#include "circles.h"

#include "types.h"

#include <gismo/gismo.h>

using namespace Document;
using namespace types;

namespace {
  SharedPtr<gismo::gsNurbs<real_t>>
  nurbs_circle(const Point& center, Vector normal,
               const Vector& xdir_radius)
  {
    normal /= sqrt(normal.squared_length());

    const Vector& xdir = xdir_radius;
    double xdirx = to_float(xdir.x());
    double xdiry = to_float(xdir.y());
    double xdirz = to_float(xdir.z());

    Vector ydir = cross_product(normal, xdir);
    double ydirx = to_float(ydir.x());
    double ydiry = to_float(ydir.y());
    double ydirz = to_float(ydir.z());

    gismo::gsKnotVector<real_t> KV2 (0,1,3,3,2) ;
    gismo::gsMatrix<real_t> C(9,3) ;
    C <<
      xdirx,       xdiry,       xdirz,
      xdirx+ydirx, xdiry+ydiry, xdirz+ydirz,
      ydirx,       ydiry,       ydirz,
      ydirx-xdirx, ydiry-xdiry, ydirz-xdirz,
      -xdirx,       -xdiry,       -xdirz,
      -xdirx-ydirx, -xdiry-ydiry, -xdirz-ydirz,
      -ydirx,       -ydiry,       -ydirz,
      xdirx-ydirx, xdiry-ydiry, xdirz-ydirz,
      xdirx,       xdiry,       xdirz;

    C.col(0).array() += to_float(center.x());
    C.col(1).array() += to_float(center.y());
    C.col(2).array() += to_float(center.z());

    gismo::gsMatrix<real_t> ww(9,1) ;
    ww <<
      1, 0.707106781186548,
      1, 0.707106781186548,
      1, 0.707106781186548,
      1, 0.707106781186548, 1;

    return std::make_shared<gismo::gsNurbs<real_t>>(KV2, give(ww), give(C));
  }

  SharedPtr<gismo::gsNurbs<real_t>>
  nurbs_circle_3_points(const Point& A, const Point& B,
                        const Point& C)
  {
    auto u = C - B;
    auto v = A - C;
    auto w = B - A;

    auto a = u.squared_length() * dot_product(v, w);
    auto b = v.squared_length() * dot_product(w, u);
    auto c = w.squared_length() * dot_product(u, v);

    auto center = (a*A.to_vector() + b*B.to_vector() + c*C.to_vector()).to_point();
    auto normal = cross_product(u, v);

    return nurbs_circle(center, std::move(normal), A - center);
  }
}

CirclePointRadius2Normal::CirclePointRadius2Normal(Point center, Real radius2, Vector normal)
    : Exporter(std::move(center), std::move(normal), std::move(radius2))
{
}

SharedPtr<CirclePointRadius2Normal> CirclePointRadius2Normal::deepCopy() const
{
  Threads::ReaderGate gate{*this};
  return SharedPtrWrap<CirclePointRadius2Normal>(gate->center, gate->radius2, gate->normal);
}

SharedPtr<const DocumentGeometry::iga_curve_t>
CirclePointRadius2Normal::produceIgaCurve() const
{
  Point center;
  real_t radius;
  Vector normal;
  {
    Threads::ReaderGate gate{*this};
    center = gate->center;
    radius = to_float(sqrt(gate->radius2));
    normal = gate->normal;
  }

  auto absx = std::abs(normal.x());
  auto absy = std::abs(normal.y());
  auto absz = std::abs(normal.z());

  auto xdir = normal;
  if(absx <= absy && absx <= absz) {
    xdir.x = 0;
    xdir.y = -1 * normal.z;
    xdir.z = normal.y;
  } else if(absy <= absx && absy <= absz) {
    xdir.x = -1 * normal.z;
    xdir.y = 0;
    xdir.z = normal.x;
  } else {
    xdir.x = -1 * normal.y;
    xdir.y = normal.x;
    xdir.z = 0;
  }

  xdir *= (radius / to_float(sqrt(xdir.squared_length())));

  return nurbs_circle(center, std::move(normal), xdir);
}


Circle3Points::Circle3Points(Point a, Point b, Point c)
    : Exporter(std::move(a), std::move(b), std::move(c))
{
}

SharedPtr<Circle3Points> Circle3Points::deepCopy() const
{
  Threads::ReaderGate gate{*this};
  return SharedPtrWrap<Circle3Points>(gate->a, gate->b, gate->c);
}

SharedPtr<const DocumentGeometry::iga_curve_t>
Circle3Points::produceIgaCurve() const
{
  Point a, b, c;
  {
    Threads::ReaderGate gate{*this};
    a = gate->a;
    b = gate->b;
    c = gate->c;
  }
  return nurbs_circle_3_points(a, b, c);
}


/**
 * Template instantiation.
 * There is no need to include NameSearch_impl.h for those.
 */
#include <libparacadis/base/naming/IExport.h>
#include <libparacadis/base/naming/NameSearch.h>

template class Naming::IExport<CirclePointRadius2Normal>;
template class Naming::IExport<Circle3Points>;

template class Naming::NameSearch<CirclePointRadius2Normal>;
template class Naming::NameSearch<Circle3Points>;
