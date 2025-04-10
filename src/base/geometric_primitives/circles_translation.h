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

#include "circles.h"
#include "circles_description.h"
#include "deferenceables_translation.h"

#include <base/data_description/DataTranslator.h>
#include <base/data_description/GateTranslator.h>

namespace DataDescription
{
  template<>
  class DataTranslator<CirclePointRadius2NormalData, CircleRadiusCenterNormal>
  {
  public:
    using inner_t = CirclePointRadius2NormalData;
    using user_t = CircleRadiusCenterNormal;

    using point_translator_t = GateTranslator<DeferenceablePoint, FloatPoint3D>;
    using vector_translator_t = GateTranslator<DeferenceableVector, FloatVector3D>;

    DataTranslator(const inner_t& inner);

    void update(const inner_t& inner, user_t& user);
    void commit(inner_t& inner, const user_t& user);

  private:
    inner_t inner;
    user_t  user;

    point_translator_t center{inner.center, user.center};
    vector_translator_t normal{inner.normal, user.normal};
  };

  template<>
  class DataTranslator<CirclePointRadius2NormalData>
      : public DataTranslator<CirclePointRadius2NormalData, CircleRadiusCenterNormal>
  {};
}
