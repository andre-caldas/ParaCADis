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

#include "FilamentGismoMesh.h"

#include "GlThreadQueue.h"

#include <gismo/gismo.h>

#include <math/vec4.h>

#include <atomic>
#include <memory>
#include <format>

namespace {
  struct VertexSetCurve {
    filament::math::float4 point;
  };

  struct VertexSetSurface {
    filament::math::float4 point;
    filament::math::float4 normal;
  };
}


namespace Mesh
{
  FilamentGismoMesh::FilamentGismoMesh(WeakPtr<filament::Engine> _engine)
      : engine(std::move(_engine))
  {
//    Create builder;
  }


  void FilamentGismoMesh::resetIgaSlot(SharedPtr<const iga_geometry_t> iga_geometry)
  {
    if(!iga_geometry) {
      return;
    }
    auto dimension = iga_geometry->parDim();

    if(dimension == 1) {
      resetCurve(*iga_geometry);
    } else if(dimension == 2) {
      resetSurface(*iga_geometry);
    } else {
      assert(false && "Must be a curve or a surface.");
    }
  }


  void FilamentGismoMesh::resetCurve(const iga_geometry_t& iga_geometry)
  {
    auto shared_engine = engine.lock();
    if(!shared_engine) {
      std::cerr << "Engine already destroyed." << std::endl;
      return;
    }

    const gismo::gsMatrix<real_t> param = iga_geometry.parameterRange();
    // TODO: decide sample size according to some precise mathematical criteria.
    gismo::gsGridIterator<real_t, gismo::CUBE> pIter(param, 100);

    const auto np = pIter.numPointsCwise();
    const auto npoints = np[0];
    if(!npoints) {
      return;
    }

    /*
     * Data for each vertex: position, normal, color, etc.
     */
    struct curve_vdata
    {
      filament::math::float4 pos;
    };
    std::unique_ptr<std::vector<curve_vdata>> vertices;
    std::unique_ptr<std::vector<uint16_t>> indices;

    vertices->reserve(npoints);
    indices->reserve(npoints);

    /*
     * Extract and process data from G+Smo.
     */
    auto domain_points = pIter.toMatrix();
    auto _positions  = iga_geometry.eval(domain_points);
    assert(_positions.cols() == npoints
           && "Wrong number of positions predicted.");

    /*
     * Bounding box book keeping.
     */
    auto const& firstp = _positions.col(0);
    filament::math::float4 min_bound{firstp[0], firstp[1], firstp[2], 1.0f};
    filament::math::float4 max_bound = min_bound;

    for(uint16_t i=0; i < _positions.cols(); ++i) {
      auto const& pcol = _positions.col(i);
      filament::math::float4 pos{pcol[0], pcol[1], pcol[2], 1.0f};

      // Sets the bounding box.
      min_bound = min(min_bound, pos);
      max_bound = max(max_bound, pos);

      // Sets the positions
      vertices->emplace_back(pos);
      indices->push_back(i);
    }

    auto vb = filament::VertexBuffer::Builder()
              .bufferCount(1)
              .vertexCount(1)
              .attribute(filament::VertexAttribute::POSITION, 0,
                         filament::VertexBuffer::AttributeType::FLOAT4,
                         offsetof(curve_vdata, pos), sizeof(curve_vdata))
              .build(*shared_engine);

    assert(vertices->size() && "Vertex data cannot be empty.");
    vb->setBufferAt(*shared_engine, 0, filament::VertexBuffer::BufferDescriptor::make(
      vertices->data(), vertices->size() * sizeof(curve_vdata),
      [v = std::move(vertices)](void*, size_t, void*){}));

    auto ib = filament::IndexBuffer::Builder()
              .indexCount(indices->size())
              .bufferType(filament::IndexBuffer::IndexType::USHORT)
              .build(*shared_engine);

    assert(indices->size() && "Index data cannot be empty.");
    vb->setBufferAt(*shared_engine, 0, filament::IndexBuffer::BufferDescriptor::make(
      indices->data(), indices->size() * sizeof(uint16_t),
      [i = std::move(indices)](void*, size_t, void*){}));

    mesh_data = std::make_shared<FilamentMeshData>(shared_engine, vb, ib, min_bound, max_bound);
  }


  void FilamentGismoMesh::resetSurface(const iga_geometry_t& iga_geometry)
  {
    auto shared_engine = engine.lock();
    if(!shared_engine) {
      std::cerr << "Engine already destroyed." << std::endl;
      return;
    }

    gismo::gsNormalField<real_t> normal_field{iga_geometry};

    const gismo::gsMatrix<real_t> param = iga_geometry.parameterRange();
    // TODO: decide sample size according to some precise mathematical criteria.
    gismo::gsGridIterator<real_t, gismo::CUBE> pIter(param, 1000);

    const auto np = pIter.numPointsCwise();
    const auto npoints = np[0] * np[1];
    // Two per rectangle for each orientation.
    const auto ntriangles = 2 * 2 * (np[0]-1) * (np[1]-1);

    /*
     * Data for each vertex: position, normal, color, etc.
     */
    struct surface_vdata
    {
      filament::math::float4 pos;
      filament::math::float4 normal;
    };
    std::unique_ptr<std::vector<surface_vdata>> vertices;
    std::unique_ptr<std::vector<uint16_t>> indices;

    vertices->reserve(npoints);
    indices->reserve(ntriangles);

    /*
     * Extract and process data from G+Smo.
     */
    auto domain_points = pIter.toMatrix();
    auto _positions  = iga_geometry.eval(domain_points);
    auto _normals = normal_field.eval(domain_points);
    assert(_positions.cols() == npoints
           && "Wrong number of positions predicted.");
    assert(_positions.cols() == _normals.cols()
           && "We should have one normal for each vertex.");

    /*
     * Bounding box book keeping.
     */
    auto const& firstp = _positions.col(0);
    filament::math::float4 min_bound{firstp[0], firstp[1], firstp[2], 1.0f};
    filament::math::float4 max_bound = min_bound;

    for(int i=0; i < _positions.cols(); ++i) {
      auto const& pcol = _positions.col(i);
      auto const& ncol = _normals.col(i);

      filament::math::float4 pos(pcol[0], pcol[1], pcol[2], 1.0f);
      filament::math::float4 normal(ncol[0], ncol[1], ncol[2], 0.0f);
      normal /= length(normal);

      // Sets the bounding box.
      min_bound = min(min_bound, pos);
      max_bound = max(max_bound, pos);

      // Sets the positions
      vertices->emplace_back(pos, normal);
    }

    for(index_t j = 0; j < np[1]-1; ++j) {
      for(index_t i= 0; i < np[0]-1; ++i) {
        const index_t ind1 = j * np[0] + i;
        const index_t ind2 = ind1 + np[0];
        indices->push_back(ind1);
        indices->push_back(ind1+1);
        indices->push_back(ind2+1);
        indices->push_back(ind1);
        indices->push_back(ind2+1);
        indices->push_back(ind1+1);

        indices->push_back(ind2+1);
        indices->push_back(ind2);
        indices->push_back(ind1);
        indices->push_back(ind2+1);
        indices->push_back(ind1);
        indices->push_back(ind2);
      }
    }

    auto vb = filament::VertexBuffer::Builder()
              .bufferCount(1)
              .vertexCount(1)
              .attribute(filament::VertexAttribute::POSITION, 0,
                         filament::VertexBuffer::AttributeType::FLOAT4,
                         offsetof(surface_vdata, pos), sizeof(surface_vdata))
              .build(*shared_engine);

    assert(vertices->size() && "Vertex data cannot be empty.");
    vb->setBufferAt(*shared_engine, 0, filament::VertexBuffer::BufferDescriptor::make(
      vertices->data(), vertices->size() * sizeof(surface_vdata),
      [v = std::move(vertices)](void*, size_t, void*){}));

    auto ib = filament::IndexBuffer::Builder()
              .indexCount(indices->size())
              .bufferType(filament::IndexBuffer::IndexType::USHORT)
              .build(*shared_engine);

    assert(indices->size() && "Index data cannot be empty.");
    vb->setBufferAt(*shared_engine, 0, filament::IndexBuffer::BufferDescriptor::make(
      indices->data(), indices->size() * sizeof(uint16_t),
      [i = std::move(indices)](void*, size_t, void*){}));

    mesh_data = std::make_shared<FilamentMeshData>(shared_engine, vb, ib, min_bound, max_bound);
  }
}
