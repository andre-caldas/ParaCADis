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

/**
 * @file
 * A MeshProvider is a class that reads a native geometric object
 * and generates a mesh so it can be drawn in a 3D environment.
 *
 * The default mesh providers keep a reference to a native object
 * and a newly generated G+Smo object. The native object must implement
 * produceGismoCurve() or produceGismoSurface(), returning something movable
 * to a std::shared_ptr (probably, a shared_ptr or a unique_ptr).
 *
 * Therefore, we implement two auxiliary classes to handle those references.
 */

#pragma once

#include "MeshProviderConcepts.h"

#include <base/expected_behaviour/SharedPtr.h>
#include <base/geometric_primitives/types.h>
#include <base/threads/message_queue/Trigger.h>

#include <memory>
#include <atomic>

namespace Mesh
{
  /**
   * Abstract MeshProvider base.
   */
  class MeshProviderBase {};

  /**
   * Abstract MeshProvider for curves.
   */
  class MeshProviderCurve
      : public MeshProviderBase
  {
  };

  /**
   * Abstract MeshProvider for surfaces.
   */
  class MeshProviderSurface
      : public MeshProviderBase
  {
  };

  /**
   * In a highly multithreaded environment,
   * we ensure that the generated mesh data is encapsulated in an
   * atomic shared pointer to a constant mesh,
   * so that threads that still rely on old data do not crash.
   * And no one needs to wait for the data to be generated,
   * because new data is generated "off line" and it is just
   * swapped with the official when it ready for use.
   */
  template<typename MeshType>
  class MeshHolder
  {
  public:
    SharedPtr<const MeshType> getMesh();

  protected:
    void setMesh(SharedPtr<const MeshType> value);
    std::atomic<std::shared_ptr<const MeshType>> mesh;
  };


  /**
   * References (weak_ptr) a certain geometric type and triggers
   * a _callback() when the contents of this geometric type changes.
   */
  template<typename DeferenceableGeometry>
  class GeometryHolder
      : public Threads::Trigger
  {
  public:
    GeometryHolder(SharedPtr<DeferenceableGeometry> geometry)
        : geometry_weak(geometry) {callback();}

    /**
     * Connects changed signals and starts updating.
     */
    void connect(const SharedPtr<Threads::SignalQueue>& queue,
                 const SharedPtr<Threads::Trigger>& self);

  protected:
    WeakPtr<DeferenceableGeometry> geometry_weak;
  };


  /**
   * Specialize this to provide a G+Smo curve or surface mesh somehow.
   *
   * @attention You can also implement a
   * DeferenceableGeometry::produceGismoCurve() or produceGismoSurface()
   * to use a default implementation.
   */
  template<typename DeferenceableGeometry>
  class MeshProvider
  {
    MeshProvider() = delete;
  };

  /**
   * Default MeshProveiderCurve specialization for
   * objects that implement a produceGismoCurve().
   */
  template<C_ProvidesGismoCurve DeferenceableGeometry>
  class MeshProvider<DeferenceableGeometry>
      : public MeshProviderCurve
      , public MeshHolder<typename DeferenceableGeometry::curve_t>
      , public GeometryHolder<DeferenceableGeometry>
  {
  public:
    using GeometryHolder<DeferenceableGeometry>::GeometryHolder;

  protected:
    /**
     * Called whenever DeferenceableGeometry changes.
     */
    void _callback() override;
  };


  /**
   * Default MeshProveiderGismoSurface specialization for
   * objects that implement a produceGismoSurface().
   */
  template<C_ProvidesGismoSurface DeferenceableGeometry>
  class MeshProvider<DeferenceableGeometry>
      : public MeshProviderSurface
      , public MeshHolder<typename DeferenceableGeometry::surface_t>
      , public GeometryHolder<DeferenceableGeometry>
  {
  public:
    using GeometryHolder<DeferenceableGeometry>::GeometryHolder;

  protected:
    /**
     * Called whenever DeferenceableGeometry changes.
     */
    void _callback() override;
  };
}

#include "MeshProvider_impl.h"
