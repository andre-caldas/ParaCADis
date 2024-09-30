// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2024 André Caldas <andre.em.caldas@gmail.com>            *
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

#include "Signal.h"

namespace Threads
{
  /**
   * Abstract base convenience base class used to trigger an action
   * whenever a signal arrives. For now, it is used to keep generated
   * data in sync with the source data. For example, we use it to
   * update a G+Smo mesh whenever the corresponding document object changes.
   */
  class Trigger
  {
  public:
    /**
     * Connects a Trigger that generates a mesh from SourceObject.
     *
     * @attention This is not (in) a constructor because we need the object
     * to be fully constructed before connecting signals to it.
     */
    template<class SourceObject>
    void _connect(const SharedPtr<SourceObject>& from,
                  const SharedPtr<SignalQueue>& queue,
                  const SharedPtr<Trigger>& self);

    /**
     * Although harmless, copies are also not very useful,
     * because the originally connected incoming signals
     * will not be connected to this new object.
     * So, we have it forbiden by now.
     */
    /// @{
    Trigger(const Trigger&) = delete;
    Trigger(Trigger&&) = delete;
    Trigger& operator=(const Trigger&) = delete;
    Trigger& operator=(Trigger&&) = delete;
    /// @}

    virtual ~Trigger() = default;

    /**
     * To be used as the signal slot.
     * That is, this is what the signal is connected to
     * when _connect() is called.
     */
    void callback() { _callback(); changed_sig.emit_signal(); }

    mutable Signal<> changed_sig;

  protected:
    /**
     * Called when a signal from SourceObject is received.
     */
    virtual void _callback() = 0;
  };
}

#include "Trigger_impl.h"
