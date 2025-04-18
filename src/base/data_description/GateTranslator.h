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

#include <base/data_description/Description.h>
#include <base/threads/locks/gates.h>
#include <base/threads/message_queue/SignalQueue.h>

namespace DataDescription
{
  /**
   * Flag to be connected to a signal.
   *
   * You should create this using SharedPtrWrap so
   * slotInnerChanged() can be connected to a signal.
   */
  class ChangeSentinel
  {
  public:
    void slotInnerChanged();
    bool hasInnerChanged() const;

    /**
     * Must be called just before updating changes.
     */
    void resetInnerChanged();

  private:
    bool has_inner_changed = false;
  };


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
    virtual ~GateTranslatorBase() = default;

    /**
     * Attempts to synchronize all values back and forth without blocking the process.
     *
     * If virtual methods are correctly implemented,
     * data on the "user side" shall be commited only if they have actually changed.
     * This way, unchanged data shall not override changes made on the "inner side".
     *
     * @attention
     * User to inner has precedence, so it is executed first.
     */
    void trySync();

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

    /**
     * Methods to update values from the "inner side" to the "user side".
     *
     * Implementations may be selective and update only values
     * that have actually changed. But this is not a requirement.
     */
    /// @{
    /**
     * Non-blocking attempt to update values.
     */
    virtual void tryInnerToUser() = 0;
    /// Blocking version that always succeeds.
    virtual void innerToUser() = 0;
    /// @}

    /**
     * Methods to commit values from the "user side" to the "inner side".
     *
     * Implementations are REQUIRED to being selective and commit only values
     * that have actually changed.
     */
    /// @{
    /**
     * Non-blocking attempt to commit values.
     */
    virtual void tryUserToInner() = 0;
    /// Blocking version that always succeeds.
    virtual void userToInner() = 0;
    /// @}
  };


  template<Threads::C_MutexHolderWithGates Inner, typename User = void>
  class GateTranslator
    : public GateTranslatorBase
  {
    using inner_data_t = typename Inner::data_t;
    using cache_t = DataTranslator<inner_data_t, User>;
    using user_t = typename cache_t::user_t;

    static_assert(C_StructTranslator<cache_t>,
                  "Needs to define a simple translator for those structs.");

  public:
    /**
     * Constructors.
     *
     * @attention
     * May block, in order to initialize structs.
     */
    /// @{
    /**
     * Translator with user and inner caches.
     */
    GateTranslator(SharedPtr<Inner> inner);

    /**
     * Translator with inner cache that reuses an already existing user cache.
     */
    GateTranslator(SharedPtr<Inner> inner, user_t& user_cache)
      requires C_StructSubTranslator<cache_t>;
    /// @}

    void innerToUser() override;
    void tryInnerToUser() override;
    void userToInner() override;
    void tryUserToInner() override;

  protected:
    SharedPtr<Inner> inner;
    SharedPtrWrap<cache_t> cache;
    user_t user;

    SharedPtrWrap<ChangeSentinel> change_sentinel;
    SharedPtrWrap<Threads::SignalQueue> signal_queue;
  };
}

#include "GateTranslator.hpp"
