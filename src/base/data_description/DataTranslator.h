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

#include <concepts>
#include <vector>

namespace DataDescription
{
  class GateTranslatorBase;

  /**
   * Template to be specialized.
   *
   * @attention
   * If you are getting the static_assert error, this means
   * you have to specialize DataTranslator and define the proper
   * translation between the structures you are using.
   * Or, you are just forgetting to include the propper `.h` file.
   */
  template<typename Inner, typename User = void>
  class DataTranslator
  {
    static_assert(sizeof(Inner) == 0,
                  "Did you forget to include the specialization?");
  };


  /**
   * Concept of a "simple translator".
   *
   * Translates directly from a struct to another without any locks or gates.
   */
  template<typename T>
  concept C_StructTranslator = requires(T a,
                T::inner_t inner,
                T::user_t user,
                const T::inner_t const_inner,
                const T::user_t const_user) {
    // Constructs from inner struct.
    T{inner};

    // Translate from inner to user when has changes.
    a.update(const_inner, user);
    // Translate from user to inner when has changes.
    a.commit(inner, const_user);

    {a.getSubTranslators()} -> std::same_as<const std::vector<GateTranslatorBase*>&>;
  };

  /**
   * Sometimes the translator can use an already existing user cache.
   *
   * This is used, for example, by DeferenceableVectorData,
   * because other structs usually contain points or vectors.
   * When translating those points or vectors,
   * that are inside other structs, the cache already exists.
   */
  template<typename T>
  concept C_StructSubTranslator = C_StructTranslator<T> &&
    requires (const typename T::inner_t inner, T::user_t user_cache) {
    /*
     * Constructor shall
     */
    T{inner, user_cache};
  };
}
