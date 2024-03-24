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

  std::string Container::toString() const
  {
    return std::format("Container ({})", getName());
  }

  void Container::addExporter(Exporter& element)
  {
    auto ptr = dynamic_cast<Container*>(&element);
    if (ptr) {
      addContainer(*ptr);
      return;
    }

    auto& gate = non_containers.getWriterGate();
    if (!gate->contains(element.getUuid())) {
      throw ElementAlreadyInContainer(element, *this);
    }
    gate->insert({element.getUuid(), element.sharedFromThis<Exporter>()});
  }

  void Container::addContainer(Container& container)
  {
    auto& gate = containers.getWriterGate();
    if (!gate->contains(container.getUuid())) {
      throw ElementAlreadyInContainer(container, *this);
    }
    gate->insert({container.getUuid(), container.sharedFromThis<Container>()});
  }

}  // namespace DocumentTree
