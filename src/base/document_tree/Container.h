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

#include <base/expected_behaviour/SharedPtr.h>
#include <base/geometric_primitives/CoordinateSystem.h>
#include <base/naming_scheme/Exporter.h>
#include <base/naming_scheme/IExport.h>
#include <base/naming_scheme/NameAndUuid.h>
#include <base/threads/safe_structs/ThreadSafeMap.h>

namespace DocumentTree
{
  class Container
      : public NamingScheme::Exporter
      , public NamingScheme::IExport<NamingScheme::Exporter>
  {
    using Exporter = NamingScheme::Exporter;

  public:
    void addExporter(Exporter& element);
    void addContainer(Container& container);

  private:
    using uuid_type = NamingScheme::Uuid::uuid_type;
    template<typename Key, typename Val>
    using UnorderedMap = Threads::SafeStructs::ThreadSafeUnorderedMap<Key, Val>;

    UnorderedMap<uuid_type, SharedPtr<Exporter>>  non_containers;
    UnorderedMap<uuid_type, SharedPtr<Container>> containers;
  };

}  // namespace DocumentTree
