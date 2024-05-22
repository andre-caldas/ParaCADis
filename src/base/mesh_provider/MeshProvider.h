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

#include <base/expected_behaviour/SharedPtr.h>
#include <base/geometric_primitives/types.h>
#include <base/threads/message_queue/Signal.h>

#include <CGAL/Surface_mesh.h>

#include <memory>
#include <atomic>

namespace Mesh
{
  class MeshProvider
  {
  public:
    virtual ~MeshProvider() = default;

    // To be used as a signal slot.
    void recalculate() { _recalculate(); changed_sig.emit_signal(); }

    mutable Threads::Signal<> changed_sig;

  protected:
    virtual void _recalculate() = 0;
  };

  template<typename MeshType, typename ReferenceableGeometry>
  class MeshProviderT : public MeshProvider
  {
  public:
    MeshProviderT(SharedPtr<ReferenceableGeometry> geometry);
    SharedPtr<const MeshType> getMesh();

  protected:
    void setMesh(SharedPtr<const MeshType> value);

    WeakPtr<ReferenceableGeometry> geometry_weak;
    std::atomic<std::shared_ptr<const MeshType>> mesh;
  };


  class LineMesh;
  using SurfaceMesh = CGAL::Surface_mesh<Point>;

  template<typename ReferenceableGeometry>
  using MeshProviderCurve
      = MeshProviderT<LineMesh, ReferenceableGeometry>;

  template<typename ReferenceableGeometry>
  using MeshProviderSurface
      = MeshProviderT<SurfaceMesh, ReferenceableGeometry>;
}

#include "MeshProvider_impl.h"
