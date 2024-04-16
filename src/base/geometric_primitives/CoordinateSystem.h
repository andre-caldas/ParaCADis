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

#include "deferenceables.h"
#include "types.h"

#include <base/naming_scheme/Chainables.h>

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
  Point  origin = {0,0,0,1};
  Vector bx     = {1,0,0,1};
  Vector by     = {0,1,0,1};
  Vector bz     = {0,0,1,1};

public:
  CoordinateSystem() = default;
  CoordinateSystem(Point origin);
  CoordinateSystem(Point origin, Vector x, Vector y, Vector z);

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
  void set(Point origin, Vector x, Vector y, Vector z);
  void set(Point origin, Real orientation, Vector y, Vector z);
  void set(Point origin, Vector x, Real orientation, Vector z);
  void set(Point origin, Vector x, Vector y, Real orientation);
  /// @}

  /** Translate the amount `displacement`.
   *
   * There are two versions `translate_in` and `translate_out`.
   * Using the `_in` version, the `displacement` vector is "inside" the
   * coordinate system. In its turn, using the `_out` version, it is  "outside".
   */
  /// @{
  void translateIn(const Vector& displacement);
  void translateOut(const Vector& displacement);
  /// @}

  /** Move the coordinate system's origin to the indicated point.
   *
   * There are two versions `move_to_in` and `move_to_out`.
   * Using the `_in` version, the `position` point is "inside" the coordinate
   * system. In its turn, using the `_out` version, it is  "outside".
   */
  /// @{
  void moveToIn(const Point& position);
  void moveToOut(Point position);
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
  Point  transform(const Point& p) const;
  Vector transform(const Vector& v) const;

  Point  operator*(const Point& p) const { return transform(p); }
  Vector operator*(const Vector& v) const { return transform(v); }
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
  CoordinateSystem compose(const CoordinateSystem& c) const;
  CoordinateSystem operator*(const CoordinateSystem& c) const { return compose(c); }
  /// @}
};

/**
 * DataStruct for CircleRadius.
 */
struct DeferenceableCoordinateSystemData {
  DeferenceablePoint  origin = {0, 0, 0};
  DeferenceableVector bx     = {1, 0, 0};
  DeferenceableVector by     = {0, 1, 0};
  DeferenceableVector bz     = {0, 0, 1};

  DeferenceableCoordinateSystemData() = default;
  DeferenceableCoordinateSystemData(const Point& origin) : origin(origin) {}
  DeferenceableCoordinateSystemData(const Point& origin,
                                    const Vector& bx,
                                    const Vector& by,
                                    const Vector& bz)
      : origin(origin), bx(bx), by(by), bz(bz) {}
};

class DeferenceableCoordinateSystem
    : public NamingScheme::Exporter<DeferenceableCoordinateSystemData>
    , public NamingScheme::IExportStruct<DeferenceablePoint, DeferenceableCoordinateSystemData,
                                {&DeferenceableCoordinateSystemData::origin, "origin"},
                                {&DeferenceableCoordinateSystemData::origin, "o"}>
    , public NamingScheme::IExportStruct<DeferenceableVector, DeferenceableCoordinateSystemData,
                                {&DeferenceableCoordinateSystemData::bx, "bx"},
                                {&DeferenceableCoordinateSystemData::bx, "x"},
                                {&DeferenceableCoordinateSystemData::bx, "i"},
                                {&DeferenceableCoordinateSystemData::by, "by"},
                                {&DeferenceableCoordinateSystemData::by, "y"},
                                {&DeferenceableCoordinateSystemData::by, "j"},
                                {&DeferenceableCoordinateSystemData::bz, "bz"},
                                {&DeferenceableCoordinateSystemData::bz, "z"},
                                {&DeferenceableCoordinateSystemData::bz, "k"}>
    , public NamingScheme::Chainables<DeferenceablePoint, DeferenceableVector>
{
public:
  DeferenceableCoordinateSystem() = default;
  DeferenceableCoordinateSystem(Point origin) noexcept;
  DeferenceableCoordinateSystem(Point origin, Vector x, Vector y, Vector z) noexcept;
  CoordinateSystem getCoordinateSystem() const noexcept;

  std::unique_ptr<DeferenceableCoordinateSystem> deepCopy() const;
  std::unique_ptr<NamingScheme::ExporterBase> deepCopyExporter() const override
  { return deepCopy(); }
};

#endif
