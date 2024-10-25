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

#include <gismo/gsNurbs/gsBSpline.h>
#include <gismo/gsNurbs/gsNurbs.h>

using namespace gismo;
using namespace Document;

namespace {
  SharedPtr<gsNurbs<real_t>>
  nurbs_circle(const K::Circle_3& circle)
  {
    auto radius = CGAL::approximate_sqrt(circle.squared_radius());
    const auto& center = circle.center();
    const auto& plane = circle.supporting_plane();

    Vector xdir = plane.base1();
    xdir *= radius / CGAL::approximate_sqrt(xdir.squared_length());
    double xdirx = CGAL::to_double(xdir.x());
    double xdiry = CGAL::to_double(xdir.y());
    double xdirz = CGAL::to_double(xdir.z());

    Vector ydir = plane.base2();
    ydir *= radius / CGAL::approximate_sqrt(ydir.squared_length());
    double ydirx = CGAL::to_double(ydir.x());
    double ydiry = CGAL::to_double(ydir.y());
    double ydirz = CGAL::to_double(ydir.z());

    gsKnotVector<real_t> KV2 (0,1,3,3,2) ;
    gsMatrix<real_t> C(9,3) ;
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

    C.col(0).array() += CGAL::to_double(center.x());
    C.col(1).array() += CGAL::to_double(center.y());
    C.col(2).array() += CGAL::to_double(center.z());

    gsMatrix<real_t> ww(9,1) ;
    ww <<
      1, 0.707106781186548,
      1, 0.707106781186548,
      1, 0.707106781186548,
      1, 0.707106781186548, 1;

    return std::make_shared<gsNurbs<real_t>>(KV2, give(ww), give(C));
  }
}

CirclePointRadius2Normal::CirclePointRadius2Normal(Point center, Real radius2, Vector normal)
    : Exporter{{std::move(center), std::move(normal), std::move(radius2)}}
{
}

std::unique_ptr<CirclePointRadius2Normal> CirclePointRadius2Normal::deepCopy() const
{
  Threads::ReaderGate gate{*this};
  return std::make_unique<CirclePointRadius2Normal>(gate->center, gate->radius2, gate->normal);
}

SharedPtr<const DocumentGeometry::iga_curve_t>
CirclePointRadius2Normal::produceIgaCurve() const
{
  Point center;
  real_t radius2;
  Vector normal;
  {
    Threads::ReaderGate gate{*this};
    center = gate->center;
    radius2 = CGAL::to_double(gate->radius2);
    normal = gate->normal;
  }
  K::Circle_3 cgal_circle{center, radius2, normal};
  return nurbs_circle(cgal_circle);
}


Circle3Points::Circle3Points(Point a, Point b, Point c)
    : Exporter{{std::move(a), std::move(b), std::move(c)}}
{
}

std::unique_ptr<Circle3Points> Circle3Points::deepCopy() const
{
  Threads::ReaderGate gate{*this};
  return std::make_unique<Circle3Points>(gate->a, gate->b, gate->c);
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
  K::Circle_3 cgal_circle{a, b, c};
  return nurbs_circle(cgal_circle);
}


/**
 * Template instantiation.
 * There is no need to include NameSearch_impl.h for those.
 */
#include <base/naming_scheme/IExport.h>
#include <base/naming_scheme/NameSearch.h>
#include <base/naming_scheme/NameSearch_impl.h>

using namespace NamingScheme;

template class IExport<CirclePointRadius2Normal>;
template class IExport<Circle3Points>;

template class NameSearch<CirclePointRadius2Normal>;
template class NameSearch<Circle3Points>;
