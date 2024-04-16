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
    Threads::SafeStructs::ThreadSafeMap<Uuid::uuid_type, WeakPtr<ExporterBase>> map;
  }

  Uuid ExporterBase::getUuid() const
  {
    return id.getUuid();
  }

  ExporterBase::operator Uuid() const
  {
    return id.getUuid();
  }

  ExporterBase::operator Uuid::uuid_type() const
  {
    return id.getUuid();
  }


  std::string ExporterBase::getName() const
  {
    return id.getName();
  }

  void ExporterBase::setName(std::string name)
  {
    id.setName(std::move(name));
  }


  void ExporterBase::registerUuid(SharedPtr<ExporterBase> shared_ptr)
  {
    auto uuid = shared_ptr->getUuid();
    assert(uuid.isValid());
    Threads::WriterGate gate{map};
    gate->emplace(uuid, std::move(shared_ptr));
  }

}  // namespace NamingScheme

