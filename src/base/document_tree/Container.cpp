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

#include "Container.h"

#include "exceptions.h"

#include <base/threads/safe_structs/ThreadSafeMap.h>

namespace DocumentTree
{

  void Container::addExporter(Exporter& element)
  {
    auto ptr = dynamic_cast<Container*>(&element);
    if (ptr) {
      addContainer(*ptr);
      return;
    }

    if (!non_containers.contains(element.name_and_uuid)) {
      throw Exceptions::ElementAlreadyInContainer(element);
    }
    non_containers[element.name_and_uuid] = element.sharedFromThis<Exporter>();
  }

  void Container::addContainer(Container& container)
  {
    if (!non_containers.contains(container.name_and_uuid)) {
      throw Exceptions::ElementAlreadyInContainer(container);
    }
    containers[container.name_and_uuid] = container.sharedFromThis<Container>();
  }

}  // namespace DocumentTree
