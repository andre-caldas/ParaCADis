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

#include "mesh_lines.h"
#include "LineMesh.h"

#include <misplaced_constants.h>

#include <base/expected_behaviour/SharedPtr.h>
#include <base/geometric_primitives/types.h>
#include <base/threads/locks/writer_locks.h>

#include <cmath>

namespace Mesh
{

  namespace {
    SharedPtr<LineMesh> getLine(const Point& a, const Point& b)
    {
      auto v = b - a;
      v *= CONFIG_WORLD_DIAMETER * CGAL::sqrt(CGAL::squared_length(v));
      Point far_a = b - v;
      Point far_b = a + v;

      return std::make_shared<LineMesh>(far_a, a, b, far_b);
    }
  }

  void MeshLine2Points::_recalculate()
  {
    auto geometry = geometry_weak.lock();
    if(!geometry) { return; }
    Threads::WriterGate gate{*geometry};
    Point a = gate->start;
    Point b = gate->end;
    // TODO: gate.release() is important to release lock before
    // doing heavy operations.
//    gate.release();
    setMesh(getLine(a, b));
  }

  void MeshLinePointDirection::_recalculate()
  {
    auto geometry = geometry_weak.lock();
    if(!geometry) { return; }
    Threads::WriterGate gate{*geometry};
    Point a = gate->start;
    Point b = a + Vector(gate->direction);
    // TODO: gate.release() is important to release lock before
    // doing heavy operations.
//    gate.release();
    setMesh(getLine(a, b));
  }

}
