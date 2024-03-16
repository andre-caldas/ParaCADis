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

#ifndef GeometricPrimitives_CoordinateSystem
#define GeometricPrimitives_CoordinateSystem

#include "types.h"

#include <CGAL/Aff_transformation_3.h>

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
 */
class CoordinateSystem
{
private:
  using AffineTransform = Aff_transformation_3<K>;
  AffineTransform transform;

public:
  CoordinateSystem();

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
  set(const Point& origin, const Vector& x, const Vector& y, const Vector& z);
  set(const Point& origin, Real orientation, const Vector& y, const Vector& z);
  set(const Point& origin, const Vector& x, Real orientation, const Vector& z);
  set(const Point& origin, const Vector& x, const Vector& y, Real orientation);
  /// @}

  /** Translate the amount `displacement`.
   *
   * There are two versions `translate_in` and `translate_out`.
   * Using the `_in` version, the `displacement` vector is "inside" the
   * coordinate system. In its turn, using the `_out` version, it is  "outside".
   */
  /// @{
  void translate_in(Vector displacement);
  void translate_out(Vector displacement);
  /// @}

  /** Move the coordinate system's origin to the indicated point.
   *
   * There are two versions `move_to_in` and `move_to_out`.
   * Using the `_in` version, the `position` point is "inside" the coordinate
   * system. In its turn, using the `_out` version, it is  "outside".
   */
  /// @{
  void move_to_in(Point position);
  void move_to_out(Point position);
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
  Point  transform_point(Point p) const;
  Vector transform_Vector(Vector v) const;
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

#endif

