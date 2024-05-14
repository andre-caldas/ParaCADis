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

#include <gismo/gsNurbs/gsNurbs.h>
#include <gismo/gsMesh2/...>

#include <memory>

namespace SceneGraph
{

  /**
   * Vurtual base to implement a bridge between ParaCADis geometric objects
   * and the SceneGraph tree leafs.
   */
  class NurbsNode
  {
  protected:
    virtual ~NurbsNode() = default;

  private:
    std::unique_ptr<gsMesh> mesh;
  };


  /**
   * Implements a bridge between ParaCADis 1D geometric objects
   * and the SceneGraph tree leafs.
   */
  class NurbsCurveNode : public NurbsNode
  {
    using nurbs_t = gismo::gsNurbs<float>;
  public:
    NurbsCurvesNode(nurbs_t nurbs) : nurbs(std::move(nurbs)) {}

  private:
    nurbs_t nurbs;
  };


  /**
   * Implements a bridge between 2D ParaCADis geometric objects
   * and the SceneGraph tree leafs.
   */
  class NurbsSurfaceNode : public NurbsNode
  {
    using nurbs_t = gismo::gsTensorNurbs<2, float>;
  public:
    NurbsSurfaceNode(nurbs_t nurbs) : nurbs(std::move(nurbs)) {}

  private:
    nurbs_t nurbs;
  };

}
