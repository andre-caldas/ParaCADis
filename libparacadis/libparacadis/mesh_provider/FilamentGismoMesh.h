// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2024-2025 André Caldas <andre.em.caldas@gmail.com>       *
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

#include <libparacadis/base/expected_behaviour/SharedPtr.h>
#include <libparacadis/base/geometric_primitives/DocumentGeometry.h>
#include <libparacadis/mesh_provider/FilamentDestroyer.h>

#include <filament/Engine.h>
#include <filament/VertexBuffer.h>
#include <filament/IndexBuffer.h>

#include <memory>
#include <mutex>

namespace Mesh
{
  using native_geometry_t = Document::DocumentGeometry;
  using iga_geometry_t = native_geometry_t::iga_geometry_t;

  /**
   * Shared data for FilamentGismoMesh.
   */
  struct FilamentMeshData
  {
    using VertexDestroyer = Mesh::FilamentDestroyer<filament::VertexBuffer>;
    using IndexDestroyer = Mesh::FilamentDestroyer<filament::IndexBuffer>;
    using float4 = filament::math::float4;
    using Engine = filament::Engine;
    using VertexBuffer = filament::VertexBuffer;
    using IndexBuffer = filament::IndexBuffer;

    FilamentMeshData(const SharedPtr<filament::Engine>& engine,
                     VertexBuffer* _vb, IndexBuffer* _ib,
                     float4 min, float4 max)
        : vb(engine, _vb), ib(engine, _ib), min_bound(min), max_bound(max) {}

    VertexDestroyer vb;
    IndexDestroyer ib;
    float4 min_bound;
    float4 max_bound;
  };

  /**
   * A mesh for the IgA geometries provided by G+Smo.
   */
  class FilamentGismoMesh
  {
  public:
    FilamentGismoMesh(WeakPtr<filament::Engine> engine);
    void resetIgaSlot(SharedPtr<const iga_geometry_t> iga_geometry);

  private:
    /**
     * Prepared data.
     * Once created and populated, those buffers shall be treated as `const`.
     * To "modify" a mesh, the SharedPtr<> shall be released and a
     * brand new set of buffers shall be created.
     */
    std::atomic<std::shared_ptr<FilamentMeshData>> mesh_data;

    WeakPtr<filament::Engine> engine;

    void resetCurve(const iga_geometry_t& iga_geometry);
    void resetSurface(const iga_geometry_t& iga_geometry);
  };
}
