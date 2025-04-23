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

#include "spheres_translation.h"

#include "types.h"

namespace DataDescription
{
  /*
   * Center and radius.
   */
  DataTranslator<SphereCenterRadius2Data, SphereCenterRadius>::
      DataTranslator(const inner_t& _inner)
      : center_tr(_inner.center.getSharedPtr(), user.center)
  {
    inner.radius2 = _inner.radius2;
    user.radius = cgal::sqrt_float(_inner.radius2);
  }

  void DataTranslator<SphereCenterRadius2Data, SphereCenterRadius>::
      update(const inner_t& _inner, user_t& _user)
  {
    if(_inner.radius2 != inner.radius2) {
      inner.radius2 = _inner.radius2;
      user.radius = cgal::sqrt_float(_inner.radius2);
      _user.radius = user.radius;
    }
  }

  void DataTranslator<SphereCenterRadius2Data, SphereCenterRadius>::
      commit(inner_t& _inner, const user_t& _user)
  {
    if(_user.radius != user.radius) {
      user.radius =  _user.radius;
      inner.radius2 = _user.radius * _user.radius;
      _inner.radius2 = inner.radius2;
    }
  }

  template class DataTranslator<SphereCenterRadius2Data, SphereCenterRadius>;


  /*
   * Center and surface point.
   */
  DataTranslator<SphereCenterSurfacePointData, SphereCenterSurfacePoint>::
      DataTranslator(const inner_t& _inner)
      : center_tr(_inner.center.getSharedPtr(), user.center)
      , surface_point_tr(_inner.surface_point.getSharedPtr(), user.surface_point)
  {
  }

  void DataTranslator<SphereCenterSurfacePointData, SphereCenterSurfacePoint>::
      update(const inner_t& /*_inner*/, user_t& /*_user*/)
  {
  }

  void DataTranslator<SphereCenterSurfacePointData, SphereCenterSurfacePoint>::
      commit(inner_t& /*_inner*/, const user_t& /*_user*/)
  {
  }

  template class DataTranslator<SphereCenterSurfacePointData, SphereCenterSurfacePoint>;
}
