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

#ifndef Threads_Gates_H
#define Threads_Gates_H

#include "MutexData.h"

#include <concepts>
#include <memory>

namespace Threads
{

  class MutexData;

  template<C_MutexData MData>
  MutexData& getMutex(MData& mutex) { return mutex; }
  template<C_MutexHolder Holder>
  Holder::mutex_data_t& getMutex(const Holder& holder) { return holder; }

  /**
   * Base class for all gates.
   */
  template<typename LockType>
  class _GateBase
  {
  public:
    _GateBase(MutexData& mutex);
    _GateBase(_GateBase&&) = default;

    /**
     * @brief Releases the lock.
     */
    void release();

    /**
     * @brief Re acquires the shared lock and resumes processing.
     */
    void resume();

  private:
    MutexData& mutex;
    std::unique_ptr<LockType> lock;
  };

}  // namespace Threads

#endif
