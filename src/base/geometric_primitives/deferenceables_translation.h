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

#include "deferenceables.h"
#include "deferenceables_description.h"

#include <base/data_description/DataTranslator.h>
#include <base/data_description/GateTranslator.h> // So the user doesn't have to include.

namespace DataDescription
{
  template<C_TripletStruct TripletStruct, typename FloatTripletStruct>
  class DataTranslator<TripletStruct, FloatTripletStruct>
  {
  public:
    using inner_t = TripletStruct;
    using user_t = FloatTripletStruct;

    /**
     * Constructs a translator with inner and user cache.
     * The caches are initialized according to @a inner.
     */
    DataTranslator(const inner_t& inner);

    /**
     * Constructs a translator with inner cache.
     * The user cache is provided by @a user_cache.
     * The caches are initialized according to @a inner.
     */
    DataTranslator(const inner_t& inner, user_t& user_cache);

    void update(const inner_t& inner, user_t& user);
    void commit(inner_t& inner, const user_t& user);

    const auto& getSubTranslators() const
    {return sub_translators;}

    inner_t inner;
    user_t _local_user;
    user_t& user = _local_user;

  private:
    void init(const inner_t& inner);
    const std::vector<GateTranslatorBase*> sub_translators = {};
  };

  template<>
  struct DataTranslator<DeferenceablePointData>
      : public DataTranslator<DeferenceablePointData, FloatPoint3D>
  {
    using Base = DataTranslator<DeferenceablePointData, FloatPoint3D>;
    using Base::inner_t;
    using Base::user_t;
  };

  static_assert(C_StructTranslator<DataTranslator<DeferenceablePointData>>,
                "Translator satisfies C_StructTranslator.");

  template<>
  struct DataTranslator<DeferenceableVectorData>
      : public DataTranslator<DeferenceableVectorData, FloatVector3D>
  {
    using Base = DataTranslator<DeferenceableVectorData, FloatVector3D>;
    using Base::inner_t;
    using Base::user_t;
  };

  static_assert(C_StructTranslator<DataTranslator<DeferenceableVectorData>>,
                "Translator satisfies C_StructTranslator.");
}
