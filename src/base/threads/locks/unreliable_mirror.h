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

#include "gates.h"

#include <base/expected_behaviour/SharedPtr.h>

namespace Threads
{
  /**
   * The UnreliableMirrorBase base class is used when you don't
   * want to block waiting for a mutex lock.
   *
   * A copy (mirror) of the protected structure is kept.
   * The mirrored value can be updated (read from original)
   * and commited (written to original), but those are
   * non-blocking operations and may fail.
   *
   * This class was created to deal with ImGUI.
   * We want to pass pointers to members of the protected structure,
   * but we do not want to block the rendering thread.
   *
   * @attention
   * Destructor **must** block until update and commit are successful.
   *
   * @attention
   * Different from other gates, descendants of `UnreliableMirrorBase`
   * are supposed to live long. Therefore, those descendants must
   * warrant (by hoding a shared pointer) that the mirrored resource
   * remains valid while the mirror exists.
   */
  class UnreliableMirrorBase
  {
  public:
    virtual ~UnreliableMirrorBase() = default;

    virtual void try_update() = 0;
    virtual void try_commit() = 0;
  };


  /**
   * Mirrors the data held by a C_MutexHolderWithGates.
   *
   * @attention
   * Be sure that the mirrored data is not expensive to compare and copy.
   */
  template<C_MutexHolderWithGates Holder>
  class UnreliableMirrorGate<Holder>
    : public UnreliableMirrorBase
  {
  public:
    UnreliableMirrorGate(SharedPtr<Holder> holder);
    ~UnreliableMirrorGate();

    auto& operator*();
    auto* operator->() { return &**this; }

    void try_update() override;
    void try_commit() override;

  private:
    SharedPtr<Holder> holder;
    Holder::GateInfo::data_t mirror;
    Holder::GateInfo::data_t old_mirror;
  };
}  // namespace Threads

#include "unreliable_mirror.hpp"
