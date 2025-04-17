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

#pragma once

#include <memory>

#include <base/expected_behaviour/CycleGuard.h>
#include <base/expected_behaviour/SharedPtr.h>

namespace Ogre {
  class SceneNode;
}

namespace Document {
  class DocumentTree;
  class Container;
  class DocumentGeometry;
}

namespace NamingScheme {
  class ExporterCommon;
}

namespace SceneGraph {
  using document_t  = Document::DocumentTree;
  using container_t = Document::Container;
  using geometry_t  = Document::DocumentGeometry;
  using non_container_t = NamingScheme::ExporterCommon;

  class SceneRoot;
  class ContainerNode;
  class MeshNode;
}
