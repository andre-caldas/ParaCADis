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

#include "DataTranslator.h"

#include <base/data_description/DataWithDescription.h>
#include <base/threads/locks/gates.h>
#include <base/threads/message_queue/SignalQueue.h>

namespace DataDescription
{
  /**
   * Base class for all gate translators.
   *
   * We use virtual methods instead of a "concept",
   * so those methods can be easily called from python code.
   *
   * @todo
   * Do we need base class with virtual methods?
   * Do we need to call them from python code?
   */
  class GateTranslatorBase
  {
  public:
    /**
     * Attempts to synchronize all values back and forth without blocking the process.
     *
     * If virtual methods are correctly implemented,
     * data on the "user side" shall be commited only if they have actually changed.
     * This way, unchanged data shall not override changes made on the "inner side".
     *
     * @attention
     * User to inner has precedence, so it is executed first.
     *
     * @returns
     * In case of success, `true`. And `false`, otherwise;
     * where "success" means tryUserToInner() has succeeded.
     */
    bool trySync();

    /**
     * Synchronizes all values back and forth.
     *
     * If virtual methods are correctly implemented,
     * data on the "user side" shall be commited only if they have actually changed.
     * This way, unchanged data shall not override changes made on the "inner side".
     *
     * @attention
     * User to inner has precedence, so it is executed first.
     */
    void sync();

  protected:
    /**
     * Methods to update values from the "inner side" to the "user side".
     *
     * Implementations may be selective and update only values
     * that have actually changed. But this is not a requirement.
     */
    // @{
    /**
     * Non-blocking attempt to update values.
     *
     * @returns
     * In case of success, `true`. And `false`, otherwise;
     * where success means that needed changes were updated.
     * In particular, when no changes are needed, it shall return `true`.
     */
    virtual bool tryInnerToUser() = 0;
    /// Blocking version that always succeeds.
    virtual void innerToUser() = 0;
    // @}

    /**
     * Methods to commit values from the "user side" to the "inner side".
     *
     * Implementations are REQUIRED to being selective and commit only values
     * that have actually changed.
     */
    // @{
    /**
     * Non-blocking attempt to commit values.
     *
     * @returns
     * In case of success, `true`. And `false`, otherwise;
     * where success means that needed changes were updated.
     * In particular, when no changes are needed, it shall return `true`.
     */
    virtual bool tryUserToInner() = 0;
    /// Blocking version that always succeeds.
    virtual void userToInner() = 0;
    // @}
  };


  template<Threads::C_MutexHolderWithGates Inner, typename... User>
  class GateTranslator
    : public GateTranslatorBase
  {
    static_assert(sizeof...(User) <= 1,
                  "Too many 'User' types passed to template.");
    using inner_data_t = typename Inner::data_t;
    using cache_t = DataTranslator<inner_data_t, User...>;
    using user_t = typename cache_t::user_t;

    static_assert(C_SimpleTranslator<cache_t>,
                  "Needs to define a simple translator for those structs.");

  public:
    /**
     * Constructors.
     *
     * @attention
     * May block, in order to initialize structs.
     */
    // @{
    /**
     * Translator with user and inner caches.
     */
    GateTranslator(SharedPtr<Inner> inner);

    /**
     * Translator with inner cache that reuses an already existing user cache.
     */
    GateTranslator(SharedPtr<Inner> inner, user_t& user_cache)
      requires C_SimpleSubTranslator<cache_t>;
    // @}

    void innerToUser() override;
    bool tryInnerToUser() override;
    void userToInner() override;
    bool tryUserToInner() override;

  protected:
    SharedPtr<Inner> inner;
    SharedPtrWrap<cache_t> cache;
    user_t user;

    Threads::SignalQueue signal_queue;
  };
}

#include "GateTranslator.hpp"
