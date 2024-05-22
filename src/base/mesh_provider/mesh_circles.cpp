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

#include "mesh_circles.h"
#include "LineMesh.h"

#include <misplaced_constants.h>

#include <base/expected_behaviour/SharedPtr.h>
#include <base/geometric_primitives/types.h>
#include <base/threads/locks/writer_locks.h>

#include <cmath>

namespace Mesh
{
  namespace {
    SharedPtr<LineMesh> getPoint(const Point& a)
    {
      Vector epsilon{0.00001, 0.00001, 0.00001};
      return std::make_shared<LineMesh>(a, a + epsilon);
    }


    SharedPtr<LineMesh> getLine(const Point& a, const Point& b)
    {
      auto v = b - a;
      v *= CONFIG_WORLD_DIAMETER * CGAL::sqrt(CGAL::squared_length(v));
      auto far_a = b - v;
      auto far_b = a + v;

      return std::make_shared<LineMesh>(far_a, a, b, far_b);
    }


    SharedPtr<LineMesh> getCircle(
        const Real& radius2, const Point& center, const Vector& normal)
    {
      Vector b1{1, 0, 0};
      if(normal.hx() != 0 || normal.hy() != 0) {
        b1 = {-normal.y(), normal.x(), 0};
        b1 /= CGAL::sqrt(CGAL::squared_length(b1));
      }
      auto b2 = CGAL::cross_product(normal, b1);
      b2 /= CGAL::sqrt(CGAL::squared_length(b2));

      auto result = std::make_shared<LineMesh>();

      result->reserve(64);
      for(int j=0; j < 64-1; ++j) {
        Real x = std::cos((2*j)*CGAL_PI/double(64));
        Real y = std::sin((2*j)*CGAL_PI/double(64));
        result->push_back(VectorToPoint(x*b1 + y*b2));
      }
      result->push_back(result->front());
      return result;
    }
  }

  void MeshCirclePointRadius2Normal::_recalculate()
  {
    auto geometry = geometry_weak.lock();
    if(!geometry) { return; }
    Threads::WriterGate gate{*geometry};
    Real  radius2 = gate->radius2;
    Point center  = gate->center;
    Vector normal = gate->normal;
    // TODO: gate.release() is important to release lock before
    // doing heavy operations.
//    gate.release();
    if(normal == CGAL::NULL_VECTOR) {
      normal = {0, 0, 1};
    }
    setMesh(getCircle(radius2, center, normal));
  }

  void MeshCircle3Points::_recalculate()
  {
    auto geometry = geometry_weak.lock();
    if(!geometry) { return; }
    Threads::WriterGate gate{*geometry};
    Point a = gate->a;
    Point b = gate->b;
    Point c = gate->c;
    // TODO: gate.release() is important to release lock before
    // doing heavy operations.
//    gate.release();
    if(CGAL::collinear(a, b, c)) {
      if(a != b){
        setMesh(getLine(a, b));
      } else if(a != c) {
        setMesh(getLine(a, c));
      } else if(b != c) {
        setMesh(getLine(b, c));
      } else {
        setMesh(getPoint(a));
      }
      return;
    }

    auto center  = CGAL::circumcenter(a, b, c);
    auto radius2 = CGAL::squared_distance(center, a);
    auto normal  = CGAL::cross_product(c-b, a-b);
    setMesh(getCircle(radius2, center, normal));
  }
}
