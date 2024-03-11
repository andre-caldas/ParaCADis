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

#include <glm/gtc/quaternion.hpp>
#include <glm/vec3>

/** Stores and manages the placement of a coordinate system.
 *
 * Vectors need to be interpreted in some context.
 * We can think there is an "abstract" global coordinate system.
 * By "abstract", we mean it is not managed by @class CoordinateSystem.
 * To place a coordinate system `C` inside some other coordinate system `G`,
 * we need to specify how points in `C` are converted to points in `G`.
 * This is what @class CoordinateSyste does.
 *
 * In practice, the user, however, does not care about "how to convert".
 * For the user, this is basically a @class Container.
 * The user wants to put things inside this coordinate and have them
 * to appear on the right place. Also, the user wants methods
 * to place and manipulate the whole "container" in such a way that
 * everything inside moves along.
 *
 * We use double precision when storing and transforming the values
 * to minimize the error of incremental changes.
 *
 * @attention **Small rant (sorry!):**
 * Please, stop talking about quaternions, matrices, column vectors,
 * row vectors, etc.. Of course, it is okay to talk about quaternions
 * and linear/affine transforms (or even matrices) if you are talking
 * about the internal implementation. The public interface shall be
 * geometric (like "mid-school-geometric") and intuitive.
 */
class CoordinateSystem
{
private:
  /// We also deal with reflections. So we need an extra bit for that: 1.0 or -1.0.
  Real       orientation = 1.0;
  glm::dquat rotation    = 1.0;
  glm::dvec3 origin;

public:
  /** Places the coordinate system at @a origin,
   * with the x-axis pointing in direction @a x, etc.
   *
   * Optionally, you can pass 1.0 or -1.0 instead of one of @a x, @a y or @a z.
   * In this case, the missing vector will be generated according to the
   * "right-hand rule" (cross product / determinant) and the orientation.
   *
   * Vectors @a x, @a y and @a z do not need to be unitary, but cannot be null.
   * The axis vectors do need to be orthogonal.
   */
  /// @{
  void set(PrecisePoint origin, PreciseVector x, PreciseVector y, PreciseVector z);
  void set(PrecisePoint origin, Real orientation, PreciseVector y, PreciseVector z);
  void set(PrecisePoint origin, PreciseVector x, Real orientation, PreciseVector z);
  void set(PrecisePoint origin, PreciseVector x, PreciseVector y, Real orientation);
  /// @}

  /** Rotate about an oriented straight line (axis/base point).
   *
   * There are two versions `rotate_in` and `rotate_out`.
   * Using the `_in` version, vectors and points are "inside" the coordinate system.
   * In its turn, using the `_out` version, vectors and points are "outside".
   */
  /// @{
  void rotate_in(PreciseVector axis, Real angle, PrecisePoint base = PrecisePoint());
  void rotate_out(
      PreciseVector axis, Real angle, PrecisePoint base = PrecisePoint());
  /// @}

  /** Translate the amount `displacement`.
   *
   * There are two versions `translate_in` and `translate_out`.
   * Using the `_in` version, the `displacement` vector is "inside" the
   * coordinate system. In its turn, using the `_out` version, it is  "outside".
   */
  /// @{
  void translate_in(PreciseVector displacement);
  void translate_out(PreciseVector displacement);
  /// @}

  /** Move the coordinate system's origin to the indicated point.
   *
   * There are two versions `move_to_in` and `move_to_out`.
   * Using the `_in` version, the `position` point is "inside" the coordinate
   * system. In its turn, using the `_out` version, it is  "outside".
   */
  /// @{
  void move_to_in(PrecisePoint position);
  void move_to_out(PrecisePoint position);
  /// @}

  /** Apply the corresponding transform to points and vectors.
   *
   * A point has a position. A vector does not.
   * So, when we transform a vector,
   * we do not consider the coordinate system's origin.
   * An example of a vector is the velocity or acceleration.
   * A translation does not change vectors, only points.
   */
  /// @{
  PrecisePoint  transform_point(PrecisePoint p) const;
  PreciseVector transform_Vector(PreciseVector v) const;
  /// @}

  /** Compose two coordinate systems.
   *
   * Suppose you have a coordinate system `C` inside `B`, inside `A`.
   * When you take a point `p` and `C.transform_point(p)`, you are getting
   * `p`'s representation in the `B` coordinate system.
   * If you want to know the representation in the `A` coordinate system,
   * you have to `B.transform_point(C.transform_point(p))`.
   *
   * To compose `M = B * C`, is the same as placing `C` directly into
   * the `A` coordinate system. Now, to represent `p` in `A` coordinates,
   * you can execute only one transformation `M.transform_point(p)`.
   *
   * This might be useful in two scenarios:
   * 1. If you have to apply `C` and then `B` to many points,
   *    it is more efficient to precalculate `M = B * C`
   *    and halve the number of transforms applied.
   * 2. You might actually want to remove `C` from `B` and place it
   *    directly on `A` without changing its position.
   */
  /// @{
  CoordinateSystem compose(const CoordinateSystem c) const;
  CoordinateSystem operator*(const CoordinateSystem c) const;
  /// @}
};

