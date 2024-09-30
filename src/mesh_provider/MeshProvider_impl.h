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
  /*
   * MeshHolder
   */
  template<typename MeshType>
  SharedPtr<const MeshType>
  MeshHolder<MeshType>::getMesh()
  {
    assert(mesh && "There must always exist a valid mesh. Mesh not initialized!");
    return mesh;
  }

  template<typename MeshType>
  void MeshHolder<MeshType>::setMesh(SharedPtr<const MeshType> value)
  {
    assert(value && "Cannot set mesh to invalid pointer.");
    mesh = value.sliced();
  }


  /*
   * GeometryHolder
   */
  template<typename DeferenceableGeometry>
  void GeometryHolder<DeferenceableGeometry>::connect(
      const SharedPtr<Threads::SignalQueue>& queue,
      const SharedPtr<Threads::Trigger>& self)
  {
    auto geometry = geometry_weak.lock();
    if(!geometry) { return; }
    _connect(geometry, queue, self);
  }


  /*
   * MeshProvider
   */
  template<C_ProvidesGismoCurve DeferenceableGeometry>
  void MeshProvider<DeferenceableGeometry>::_callback()
  {
    // I really do not understand why we cannot write just "geometry_week".
    // Why do we need to fully qualify it???
    auto geometry = GeometryHolder<DeferenceableGeometry>::geometry_weak.lock();
    if(!geometry) { return; }
    setMesh(geometry->getGismoCurve());
  }


  /*
   * MeshProvider
   */
  template<C_ProvidesGismoSurface DeferenceableGeometry>
  void MeshProvider<DeferenceableGeometry>::_callback()
  {
    // I really do not understand why we cannot write just "geometry_week".
    // Why do we need to fully qualify it???
    auto geometry = GeometryHolder<DeferenceableGeometry>::geometry_weak.lock();
    if(!geometry) { return; }
    setMesh(geometry->getGismoSurface());
  }
}
