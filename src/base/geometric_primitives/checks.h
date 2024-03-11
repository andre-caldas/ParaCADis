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

#include "types.h"

namespace glm {
class mat3;
}

namespace Check
{
  bool epsilonZero(Real a);
  bool epsilonEqual(Real a, Real b);

  void assertOrthogonality(PreciseVector x, PreciseVector y);
  void assertTwoByTwoOrthogonality(PreciseVector x, PreciseVector y, PreciseVector z);

  /** If not linearly independent, throws @class NeedsLI.
  */
  /// @{
  /// @return The cross-product.
  PreciseVector assertLI(PreciseVector x, PreciseVector y);
  /// @return The determinant.
  PreciseReal assertLI(PreciseVector x, PreciseVector y, PreciseVector z);
  /// @return The determinant.
  PreciseReal assertLI(glm::mat3 M);
  /// @{
}

