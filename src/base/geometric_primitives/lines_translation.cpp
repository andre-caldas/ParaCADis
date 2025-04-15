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

#include "lines_translation.h"

#include <CGAL/number_utils.h>

namespace {
  void _construct(auto& _inner, auto& inner, auto& user) {
    inner.is_bounded_start = _inner.is_bounded_start;
    user.is_bounded_start = _inner.is_bounded_start;

    inner.is_bounded_end = _inner.is_bounded_end;
    user.is_bounded_end = _inner.is_bounded_end;
  }

  void _update(auto& _inner, auto& inner, auto& user, auto& _user)
  {
    if(_inner.is_bounded_start != inner.is_bounded_start) {
      inner.is_bounded_start = _inner.is_bounded_start;
      user.is_bounded_start = _inner.is_bounded_start;
      _user.is_bounded_start = user.is_bounded_start;
    }
    if(_inner.is_bounded_end != inner.is_bounded_end) {
      inner.is_bounded_end = _inner.is_bounded_end;
      user.is_bounded_end = _inner.is_bounded_end;
      _user.is_bounded_end = user.is_bounded_end;
    }
  }

  void _commit(auto& _inner, auto& inner, auto& user, auto& _user)
  {
    if(_user.is_bounded_start != user.is_bounded_start) {
      user.is_bounded_start =  _user.is_bounded_start;
      inner.is_bounded_start = _user.is_bounded_start;
      _inner.is_bounded_start = inner.is_bounded_start;
    }
  }
}

namespace DataDescription
{
  /*
   * Two points.
   */
  DataTranslator<Line2PointsData, Line2Points>::
      DataTranslator(const inner_t& _inner)
      : start_tr(_inner.start.getSharedPtr(), user.start)
      , end_tr(_inner.end.getSharedPtr(), user.end)
  {
    _construct(_inner, inner, user);
  }

  void DataTranslator<Line2PointsData, Line2Points>::
      update(const inner_t& _inner, user_t& _user)
  {
    _update(_inner, inner, user, _user);
  }

  void DataTranslator<Line2PointsData, Line2Points>::
      commit(inner_t& _inner, const user_t& _user)
  {
    _commit(_inner, inner, user, _user);
  }

  template class DataTranslator<Line2PointsData, Line2Points>;


  /*
   * Point and direction.
   */
  DataTranslator<LinePointDirectionData, LinePointDirection>::
      DataTranslator(const inner_t& _inner)
      : start_tr(_inner.start.getSharedPtr(), user.start)
      , direction_tr(_inner.direction.getSharedPtr(), user.direction)
  {
    _construct(_inner, inner, user);
  }

  void DataTranslator<LinePointDirectionData, LinePointDirection>::
      update(const inner_t& _inner, user_t& _user)
  {
    _update(_inner, inner, user, _user);
  }

  void DataTranslator<LinePointDirectionData, LinePointDirection>::
      commit(inner_t& _inner, const user_t& _user)
  {
    _commit(_inner, inner, user, _user);
  }

  template class DataTranslator<LinePointDirectionData, LinePointDirection>;
}
