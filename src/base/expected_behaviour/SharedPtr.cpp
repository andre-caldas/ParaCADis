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

#include "SharedPtr.h"

JustLockPtr& JustLockPtr::operator= (const JustLockPtr& other)
{
  assert(!lock && "Cannot attribute to an already locked lock.");
  lock = other.lock;
  return *this;
}

JustLockPtr& JustLockPtr::operator= (JustLockPtr&& other)
{
  assert(!lock && "Cannot attribute to an already locked lock.");
  lock = std::move(other.lock);
  return *this;
}

JustLockPtr::JustLockPtr(std::shared_ptr<void>&& ptr) : lock(std::move(ptr))
{
  assert(lock && "You cannot hold an unlocked shared_ptr.");
}


#include <base/naming_scheme/Exporter.h>

template class SharedPtr<NamingScheme::ExporterBase>;
