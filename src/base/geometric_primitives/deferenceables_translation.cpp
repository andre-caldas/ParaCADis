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

#include "deferenceables_translation.h"

namespace DataDescription
{
  template<typename FloatTripletStruct>
  DataTranslator<TripletStruct, FloatTripletStruct>::DataTranslator(const inner_t& _inner)
  {
    init(_inner);
  }


  template<typename FloatTripletStruct>
  DataTranslator<TripletStruct, FloatTripletStruct>::
      DataTranslator(const inner_t& _inner, user_t& user_cache)
      : user(user_cache)
  {
    init(_inner);
  }


  template<typename FloatTripletStruct>
  void DataTranslator<TripletStruct, FloatTripletStruct>::
      init(const inner_t& _inner)
  {
    inner = _inner;
    user.x = static_cast<float>(CGAL::to_double(_inner.x));
    user.y = static_cast<float>(CGAL::to_double(_inner.y));
    user.z = static_cast<float>(CGAL::to_double(_inner.z));
  }


  template<typename FloatTripletStruct>
  void DataTranslator<TripletStruct, FloatTripletStruct>::
      update(const inner_t& _inner, user_t& _user)
  {
    if(!hasInnerChanged()) {
      return;
    }
    resetInnerChanged();

    if(inner.x != _inner.x) {
      inner.x = _inner.x;
      user.x = static_cast<float>(CGAL::to_double(_inner.x));
      _user.x = user.x;
    }
    if(inner.y != _inner.y) {
      inner.y = _inner.y;
      user.y = static_cast<float>(CGAL::to_double(_inner.y));
      _user.y = user.y;
    }
    if(inner.z != _inner.z) {
      inner.z = _inner.z;
      user.z = static_cast<float>(CGAL::to_double(_inner.z));
      _user.z = user.z;
    }
  }


  template<typename FloatTripletStruct>
  void DataTranslator<TripletStruct, FloatTripletStruct>::
      commit(inner_t& _inner, const user_t& _user)
  {
    if(user.x != _user.x) {
      user.x = _user.x;
      inner.x = _user.x;
      _inner.x = inner.x;
    }
    if(user.y != _user.y) {
      user.y = _user.y;
      inner.y = _user.y;
      _inner.y = inner.y;
    }
    if(user.z != _user.z) {
      user.z = _user.z;
      inner.z = _user.z;
      _inner.z = inner.z;
    }
  }

  template class DataTranslator<TripletStruct, FloatPoint3D>;
  template class DataTranslator<TripletStruct, FloatVector3D>;
}
