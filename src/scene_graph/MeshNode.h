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

#include <base/geometric_primitives/DocumentGeometry.h>
#include <mesh_provider/MeshProvider.h>

#include <memory>

namespace Ogre {
  class Mesh;
}

namespace SceneGraph
{
  /**
   * Vurtual base to implement a bridge between ParaCADis geometric objects
   * and the SceneGraph tree leafs.
   */
  class MeshNode
  {
    using MeshProvider        = Mesh::MeshProviderBase;
    using MeshProviderCurve   = Mesh::MeshProviderCurve;
    using MeshProviderSurface = Mesh::MeshProviderSurface;

  public:
    static SharedPtr<MeshNode> make_shared(SharedPtr<MeshProvider> mesh_provider);

  private:
    SharedPtr<Ogre::Mesh> getOgreMesh();

    /**
     * To be attached to a signal that indicates geometry changes.
     */
    void updateMesh();

    MeshNode(SharedPtr<MeshProvider> mesh_provider)
        : mesh_provider(std::move(mesh_provider)) {}

    SharedPtr<MeshProvider> mesh_provider;
  };
}
