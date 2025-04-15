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

#pragma once

#include "spheres.h"
#include "spheres_description.h"
#include "deferenceables_translation.h"

#include <base/data_description/DataTranslator.h>
#include <base/data_description/GateTranslator.h>

namespace DataDescription
{
  /*
   * Center and radius.
   */
  template<>
  class DataTranslator<SphereCenterRadius2Data, SphereCenterRadius>
  {
  public:
    using inner_t = SphereCenterRadius2Data;
    using user_t = SphereCenterRadius;

    using point_translator_t = GateTranslator<DeferenceablePoint, FloatPoint3D>;
    using vector_translator_t = GateTranslator<DeferenceableVector, FloatVector3D>;

    DataTranslator(const inner_t& inner);

    void update(const inner_t& inner, user_t& user);
    void commit(inner_t& inner, const user_t& user);

    constexpr auto& getSubTranslators() const
    {return sub_translators;}

  private:
    inner_t inner;
    user_t  user;

    point_translator_t center_tr;
    const std::vector<GateTranslatorBase*> sub_translators = {&center_tr};
  };

  template<>
  struct DataTranslator<SphereCenterRadius2Data>
      : public DataTranslator<SphereCenterRadius2Data, SphereCenterRadius>
  {
    using Base = DataTranslator<SphereCenterRadius2Data, SphereCenterRadius>;
    using Base::inner_t;
    using Base::user_t;
  };


  /*
   * Center and surface point.
   */
  template<>
  class DataTranslator<SphereCenterSurfacePointData, SphereCenterSurfacePoint>
  {
  public:
    using inner_t = SphereCenterSurfacePointData;
    using user_t = SphereCenterSurfacePoint;

    using point_translator_t = GateTranslator<DeferenceablePoint, FloatPoint3D>;
    using vector_translator_t = GateTranslator<DeferenceableVector, FloatVector3D>;

    DataTranslator(const inner_t& inner);

    void update(const inner_t& inner, user_t& user);
    void commit(inner_t& inner, const user_t& user);

    constexpr auto& getSubTranslators() const
    {return sub_translators;}

  private:
    inner_t inner;
    user_t  user;

    point_translator_t center_tr;
    point_translator_t surface_point_tr;
    const std::vector<GateTranslatorBase*> sub_translators = {&center_tr, &surface_point_tr};
  };

  template<>
  struct DataTranslator<SphereCenterSurfacePointData>
      : public DataTranslator<SphereCenterSurfacePointData, SphereCenterSurfacePoint>
  {
    using Base = DataTranslator<SphereCenterSurfacePointData, SphereCenterSurfacePoint>;
    using Base::inner_t;
    using Base::user_t;
  };
}
