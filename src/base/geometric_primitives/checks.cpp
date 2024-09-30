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

namespace Check
{

  void assertOrthogonality(Vector x, Vector y)
  {
    if (scalar_product(x, y) != 0) {
      throw Exception::NeedsOrthogonal(x, y);
    }
  }

  void assertTwoByTwoOrthogonality(Vector x, Vector y, Vector z)
  {
    assertOrthogonality(x, y);
    assertOrthogonality(y, z);
    assertOrthogonality(z, x);
  }

  Vector assertLI(Vector x, Vector y)
  {
    auto z = cross_product(x, y);
    if (z == CGAL::NULL_VECTOR) {
      throw Exception::NeedsLI(x, y);
    }
    return z;
  }

  Real assertLI(Vector x, Vector y, Vector z)
  {
    auto det = determinant(x,y,z);
    if(det == 0) {
      throw Exception::NeedsLI(x, y, z);
    }
    return det;
  }

}  // namespace Check

