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
#include "NameAndUuid.h"

#include <base/threads/safe_structs/ThreadSafeMap.h>
#include <base/threads/locks/LockPolicy.h>

namespace NamingScheme
{

  namespace
  {
    Threads::SafeStructs::ThreadSafeMap<Uuid::uuid_type, std::weak_ptr<Exporter>> map;
  }

  Exporter::Exporter(std::string name, std::vector<std::string> aliases)
      : name_and_uuid(std::move(name))
      , name_and_aliases(std::move(aliases))
  {
    if(name_and_aliases.empty())
    {
      name_and_aliases.emplace_back(name_and_uuid.getName());
    }
  }

  void Exporter::registerUuid(const SharedPtr<Exporter>& shared_ptr)
  {
    auto uuid = shared_ptr->getUuid();
    assert(uuid.isValid());
    auto l = map.getWriter
    map.emplace({uuid, shared_ptr});
  }

  SharedPtr<Exporter> Exporter::getSharedPtr(std::string_view uuid)
  {
    Uuid t(uuid);
    return getSharedPtr(t);
  }

  SharedPtr<Exporter> Exporter::getSharedPtr(Uuid::uuid_type uuid)
  {
    SharedLock lock(map);
    if (lock->count(uuid) == 0) {
      return SharedPtr<Exporter>();
    }
    return lock->at(uuid).lock();
  }

}  // namespace NamingScheme

