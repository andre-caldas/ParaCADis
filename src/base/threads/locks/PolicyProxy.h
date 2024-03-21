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

#ifndef Threads_PolicyProxy_H
#define Threads_PolicyProxy_H

#include "LockPolicy.h"
#include "MutexData.h"

namespace Threads
{

  /**
   * Do not allow variable access if mutex is not locked.
   *
   * @example In order to allow template deduction,
   * you can use the constructor. The "this" pointer is passed
   * ```
   * class A
   * {
   *   // ...
   *   // With default MyType constructor:
   *   DataProxy<MyType> proxied_mytype = {this};
   *   // With MyType(arg1, arg2):
   *   DataProxy<MyType> proxied_mytype = {this, arg1, arg2};
   * };
   * ```
   */
  template<typename T, C_MutexHolder MutexHolder, MutexHolder* holder>
  class DataProxy
  {
  public:
    template<typename... Args>
    DataProxy(MutexHolder*, Args... args);
    template<typename... Args>
    DataProxy(Args... args);

    const T& read() const;
    T& write() const;

  private:
    T data;
  };

}  // namespace Threads

#include "LockPolicy_inl.h"

#endif
