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

#include <glm/vec3.hpp>

/** Single precision scalars, vectors and points
 *
 * @attention Although we use different names for points and vectors,
 * they are actually the same type. So, care must be taken.
 * Adding two vectors or a point and a vector is a meaningful operation.
 * Adding two points, however, is usually not very meaningful.
 */
 /// *{
typedef float     Real;
typedef glm::vec3 Point;
typedef glm::vec3 Vector;
typedef glm::vec2 Point2;
typedef glm::vec2 Vector2;
 /// *}


/** Double precision scalars, vectors and points.
 *
 * @attention If you don't have a good reason to use double precision,
 * prefer single precision, instead.
 *
 * @attention Although we use different names for points and vectors,
 * they are actually the same type. So, care must be taken.
 * Adding two vectors or a point and a vector is a meaningful operation.
 * Adding two points, however, is usually not very meaningful.
 */
 /// *{
typedef double     PreciseReal;
typedef glm::dvec3 PrecisePoint;
typedef glm::dvec3 PreciseVector;
typedef glm::dvec2 PrecisePoint2;
typedef glm::dvec2 PreciseVector2;
 /// *}

