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

#include <base/document_tree/Container.h>

#include <base/mesh_provider/MeshProvider.h>

#include <memory>

namespace SceneGraph
{

  /**
   * Vurtual base to implement a bridge between ParaCADis geometric objects
   * and the SceneGraph tree leafs.
   */
  class MeshNode
  {
  protected:
    virtual ~MeshNode() = default;
  };


  /**
   * Implements a bridge between ParaCADis 1D geometric objects
   * and the SceneGraph tree leafs.
   */
  class MeshCurveNode : public MeshNode
  {
    using mesh_t = Mesh::MeshProviderCurve;
  public:
    MeshCurveNode(mesh_t mesh) : mesh(std::move(mesh)) {}

  private:
    mesh_t mesh;
  };


  /**
   * Implements a bridge between 2D ParaCADis geometric objects
   * and the SceneGraph tree leafs.
   */
  class MeshSurfaceNode : public MeshNode
  {
    using mesh_t = Mesh::MeshProviderSurface;
  public:
    MeshSurfaceNode(mesh_t mesh) : mesh(std::move(mesh)) {}

  private:
    mesh_t mesh;
  };

}
