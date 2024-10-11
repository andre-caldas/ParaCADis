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

#include "MeshNode.h"

#include <base/geometric_primitives/DocumentGeometry.h>
#include <base/geometric_primitives/types.h>

namespace SceneGraph
{

  SharedPtr<MeshNode> MeshNode::make_shared(SharedPtr<Mesh::MeshProvider> mp)
  {
    return SharedPtr<MeshNode>::from_pointer(new MeshNode(mp));
  }

  MeshNode::MeshNode(SharedPtr<Mesh::MeshProvider> mesh_provider)
      : meshProvider(std::move(mesh_provider))
  {}

  SharedPtr<Ogre::Mesh> MeshNode::getOgreMesh()
  {
    assert(meshProvider && "Shared pointer not supposed to be invalid");
    return meshProvider->getOgreMesh();
  }

}
