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

#include "Exporter.h"
#include "Exporter_impl.h"
#include "NameAndUuid.h"

#include <base/threads/locks/LockPolicy.h>
#include <base/thread_safe_structs/ThreadSafeMap.h>

namespace NamingScheme
{

  namespace
  {
    ThreadSafeStructs::ThreadSafeMap<Uuid::uuid_type, std::weak_ptr<Exporter>> map;
  }

  Uuid::uuid_type Exporter::getUuid() const
  {
    return nameAndUuid.getUuid();
  }

  void Exporter::registerUuid(const SharedPtr<Exporter>& shared_ptr)
  {
    auto uuid = ptr->getUuid();
    assert(uuid.isValid());
    map.emplace({uuid, ptr});
  }

  SharedPtr<Exporter> Exporter::getSharedPtr(std::string_view uuid)
  {
    Uuid t(uuid);
    return getSharedPtr(t);
  }

  SharedPtr<Exporter> Exporter::getSharedPtr(Uuid::uuid_type uuid)
  {
    SharedLockFreeLock lock(map);
    if (lock->count(uuid) == 0) {
      return SharedPtr<Exporter>();
    }
    return lock->at(uuid).lock();
  }

}  // namespace NamingScheme

