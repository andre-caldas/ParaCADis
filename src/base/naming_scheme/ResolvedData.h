// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2023-2024 André Caldas <andre.em.caldas@gmail.com>       *
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

#ifndef NamingScheme_ResolvedData_H
#define NamingScheme_ResolvedData_H

#include <memory>
#include <base/threads/locks/LockPolicy.h>

namespace NamingScheme
{

  class Exporter;

  /**
   * All information for the resolved ReferenceToOjbect.
   *
   * To support multithread, NamingScheme::Exporter provides a shared_mutex
   * for accessing its data. So, we keep a reference for this mutex.
   * The developer must lock the mutex before using the data.
   */
  class ResolvedDataBase
  {
  protected:
    std::shared_ptr<Exporter> exporter;

  public:
    [[maybe_unused]] [[nodiscard]]
    Threads::SharedLock lockForReading() const;

    [[maybe_unused]] [[nodiscard]]
    Threads::ExclusiveLock<Threads::MutexData> lockForWriting() const;
  };

  template<typename T>
  class ResolvedData : public ResolvedDataBase
  {
  private:
    T* data;

  public:
    const T* getDataForReading() const;
    T*       getDataForWriting() const;
  };

}  // namespace NamingScheme

#endif

