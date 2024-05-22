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

#include "MeshProvider.h"

namespace Mesh
{
  template<typename MeshType, typename ReferenceableGeometry>
  MeshProviderT<MeshType, ReferenceableGeometry>::MeshProviderT(
      SharedPtr<ReferenceableGeometry> geometry)
      : geometry_weak(geometry)
  {
    geometry->getChangedSignal().connect(recalculate);
  }

  template<typename MeshType, typename ReferenceableGeometry>
  SharedPtr<const MeshType>
  MeshProviderT<MeshType, ReferenceableGeometry>::getMesh()
  {
    if(!mesh) {
      recalculate();
    }
    assert(mesh && "Virtual method recalculate() needs to generate a valid mesh.");
    return mesh;
  }

  template<typename MeshType, typename ReferenceableGeometry>
  void MeshProviderT<MeshType, ReferenceableGeometry>::setMesh(
      SharedPtr<const MeshType> value)
  {
    assert(value && "Cannot set mesh to invalid pointer.");
    mesh = value;
  }
}
