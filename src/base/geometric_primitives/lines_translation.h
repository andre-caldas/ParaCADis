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

#include "lines.h"
#include "lines_description.h"
#include "deferenceables_translation.h"

#include <base/data_description/DataTranslator.h>
#include <base/data_description/GateTranslator.h>

namespace DataDescription
{
  /*
   * Two points.
   */
  template<>
  class DataTranslator<Line2PointsData, Line2Points>
  {
  public:
    using inner_t = Line2PointsData;
    using user_t = Line2Points;

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

    point_translator_t start_tr;
    point_translator_t end_tr;
    const std::vector<GateTranslatorBase*> sub_translators = {&start_tr, &end_tr};
  };

  template<>
  struct DataTranslator<Line2PointsData>
      : public DataTranslator<Line2PointsData, Line2Points>
  {
    using Base = DataTranslator<Line2PointsData, Line2Points>;
    using Base::inner_t;
    using Base::user_t;
  };


  /*
   * Point and direction.
   */
  template<>
  class DataTranslator<LinePointDirectionData, LinePointDirection>
  {
  public:
    using inner_t = LinePointDirectionData;
    using user_t = LinePointDirection;

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

    point_translator_t start_tr;
    vector_translator_t direction_tr;
    const std::vector<GateTranslatorBase*> sub_translators = {&start_tr, &direction_tr};
  };

  template<>
  struct DataTranslator<LinePointDirectionData>
      : public DataTranslator<LinePointDirectionData, LinePointDirection>
  {
    using Base = DataTranslator<LinePointDirectionData, LinePointDirection>;
    using Base::inner_t;
    using Base::user_t;
  };
}
