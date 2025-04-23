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

module;
#include <CGAL/Gmpz.h>
#include <CGAL/Origin.h>
#include <CGAL/Quotient.h>
#include <CGAL/Simple_homogeneous.h>
#include <CGAL/number_utils.h>
module cgal;

namespace cgal {
  double to_double(const Real& x)
  {
    return CGAL::to_double(x);
  }

  float sqrt_float(const Real& x)
  {
    return static_cast<float>(CGAL::to_double(CGAL::sqrt(x)));
  }

  Vector sqrt_float(const Vector& v, const Vector& w)
  {
    return CGAL::cross_product(v, w);
  }
}
