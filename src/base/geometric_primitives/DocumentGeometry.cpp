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

#include "DocumentGeometry.h"

using namespace Document;

SharedPtr<const DocumentGeometry::geometry_t>
DocumentCurve::getGismoGeometry() const
{
  return getGismoCurve();
}

SharedPtr<const DocumentGeometry::curve_t>
DocumentCurve::getGismoCurve() const
{
  auto loaded = gismoGeometry.load();
  if(!loaded) {
    // Not really thread-safe because if the geometry changes while
    // it is being produced, two produceGismoCurve() will be called
    // concurrently. Even if they are thread-safe,
    // you do not know if gismoGeometry will endup with the new or old geometry.
    auto result = produceGismoCurve();
    gismoGeometry = result.sliced();
    return result;
  }
  return loaded;
}


SharedPtr<const DocumentGeometry::geometry_t>
DocumentSurface::getGismoGeometry() const
{
  return getGismoSurface();
}

SharedPtr<const DocumentGeometry::surface_t>
DocumentSurface::getGismoSurface() const
{
  auto loaded = gismoGeometry.load();
  if(!loaded) {
    // Not really thread-safe because if the geometry changes while
    // it is being produced, two produceGismoSurface() will be called
    // concurrently. Even if they are thread-safe,
    // you do not know if gismoSurface will endup with the new or old geometry.
    auto result = produceGismoSurface();
    gismoGeometry = result.sliced();
    return result;
  }
  return loaded;
}
