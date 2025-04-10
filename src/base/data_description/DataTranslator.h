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

namespace DataDescription
{
  class DataTranslatorBase
  {
  public:
    void slotInnerChanged();
    bool hasInnerChanged() const;

  protected:
    /**
     * Must be called just before updating changes.
     */
    void resetInnerChanged();

  private:
    bool has_inner_changed = false;
  };


  /**
   * Template to be specialized.
   *
   * @attention
   * If you are getting linker errors, this means
   * you have to specialize DataTranslator and define the proper
   * translation between the structures you are using.
   */
  template<typename Inner, typename User = void>
  class DataTranslator;


  /**
   * Concept of a "simple translator".
   *
   * Translates directly from a struct to another without any locks or gates.
   */
  template<typename T>
  concept C_SimpleTranslator = std::derived_from<T, DataTranslatorBase>
    && requires(T a,
                T::inner_t inner,
                T::user_t user,
                const T::inner_t const_inner,
                const T::user_t const_user) {
    // Constructs from inner object.
    T{inner};

    // Translate from inner to user when has changes.
    a.update(const_inner, user);
    // Translate from user to inner when has changes.
    a.commit(inner, const_user);
  };

  /**
   * Sometimes the translator can use an already existing user cache.
   *
   * This is used, for example, by TripletStruct,
   * because other structs usually contain points or vectors.
   * When translating those points or vectors,
   * that are inside other structs, the cache already exists.
   */
  template<typename T>
  concept C_SimpleSubTranslator = C_SimpleTranslator<T> &&
    requires (const typename T::inner_t inner, T::user_t user_cache) {
    /*
     * Constructor shall
     */
    T{inner, user_cache};
  };
}
