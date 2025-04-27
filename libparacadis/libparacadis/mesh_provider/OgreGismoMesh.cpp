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

#include "OgreGismoMesh.h"

#include "GlThreadQueue.h"

#include <gismo/gismo.h>

#include <OGRE/OgreHardwareBufferManager.h>
#include <OGRE/OgreMeshManager.h>
#include <OGRE/OgreRoot.h>
#include <OGRE/OgreSubMesh.h>

#include <atomic>
#include <memory>
#include <format>

using namespace Mesh;

namespace {
  GlThreadQueue& register_queue()
  {
    static GlThreadQueue listener;
    Ogre::Root::getSingleton().addFrameListener(&listener);
    return listener;
  }

  auto& get_queue()
  {
    static GlThreadQueue& listener = register_queue();
    return listener.queue;
  }
}


OgreGismoMesh::OgreGismoMesh(std::shared_ptr<const iga_geometry_t> iga_geometry)
    : igaGeometry(std::move(iga_geometry))
{
  using namespace Ogre;

  static std::atomic<unsigned int> counter = 0;
  mesh = MeshManager::getSingleton().createManual(
      std::format("Gismo Geometry - {:05}", counter++), // Name
      RGN_DEFAULT,                                      // Group
      this                                              // Loader
  );
  mesh->setBackgroundLoaded(true);
}

void OgreGismoMesh::init()
{
  mesh->prepare();
}

void OgreGismoMesh::resetIgaGeometry(SharedPtr<const iga_geometry_t> iga_geometry)
{
  igaGeometry = iga_geometry.sliced();
  justPrepare();

  auto lambda = [weak_self = weak_from_this()]{
    auto self = weak_self.lock();
    if(!self) {
      return;
    }
    self->loadResource(self->mesh.get());
    self->mesh->_dirtyState();
  };
  get_queue().push(lambda);
}


size_t OgreGismoMesh::vertexEntriesPerPoint() const
{
  assert(dimension && "The dimension was supposed to be set.");
  return ((dimension == 1) ? 3 : 6);
}


void OgreGismoMesh::prepareResource(Ogre::Resource*)
{
  justPrepare();

  auto lambda = [weak_self = weak_from_this()]{
    auto self = weak_self.lock();
    if(!self) {
      return;
    }
      self->mesh->escalateLoading();
  };
  get_queue().push(lambda);
}

void OgreGismoMesh::justPrepare()
{
  auto igaGeo = igaGeometry.load();
  if(!igaGeo) {
    return;
  }
  dimension = igaGeo->parDim();

  if(dimension == 1) {
    prepareCurve(igaGeo);
  } else if(dimension == 2) {
    prepareSurface(igaGeo);
  } else {
    assert(false && "Must be a curve or a surface.");
    return;
  }
}

void OgreGismoMesh::loadResource(Ogre::Resource*)
{
  using namespace Ogre;
  std::scoped_lock lock{mutex};
  assert(dimension != 0 && "Parameter dimension not set.");
  assert(dimension < 3 && "Must be a curve or a surface.");

  mesh->_setBounds(AxisAlignedBox(min_bound, max_bound));

  if(!mesh->sharedVertexData) {
    setVertexData();
  }
  prepareHardwareBuffers();

  assert(!mesh->getSubMeshes().empty());

  SubMesh* sub = mesh->getSubMeshes()[0];
  sub->operationType = (dimension == 1) ? RenderOperation::OT_LINE_STRIP
                                        : RenderOperation::OT_TRIANGLE_STRIP;
  //    sub->useSharedVertices = true;
  sub->indexData->indexBuffer = ibuf;
  sub->indexData->indexStart = 0;
  sub->indexData->indexCount = indexes.size();
}


void OgreGismoMesh::prepareCurve(const std::shared_ptr<const iga_geometry_t>& igaGeo)
{
  using namespace Ogre;

  const gismo::gsMatrix<real_t> param = igaGeo->parameterRange();
  // TODO: decide sample size according to some precise mathematical criteria.
  gismo::gsGridIterator<real_t, gismo::CUBE> pIter(param, 100);

  const auto np = pIter.numPointsCwise();
  const auto npoints = np[0];

  std::vector<float> positions;
  std::vector<Ogre::uint16> local_indexes;

  auto local_min_bound = Vector3::ZERO;
  auto local_max_bound = Vector3::ZERO;

  positions.reserve(vertexEntriesPerPoint() * npoints);
  local_indexes.reserve(npoints);

  auto domain_points = pIter.toMatrix();
  // TODO: process in parallel.
  auto _positions  = igaGeo->eval(domain_points);
  assert(_positions.cols() == npoints
         && "Wrong number of positions predicted.");
  for(int i=0; i < _positions.cols(); ++i) {
    auto const& pcol = _positions.col(i);

    Vector3 pos(pcol[0], pcol[1], pcol[2]);
    // Sets the bounding box.
    local_min_bound.makeFloor(pos);
    local_max_bound.makeCeil(pos);

    // Sets the positions
    positions.push_back(pos[0]);
    positions.push_back(pos[1]);
    positions.push_back(pos[2]);

    local_indexes.push_back(i);
  }

  std::scoped_lock lock{mutex};
  vertex  = std::move(positions);
  indexes = std::move(local_indexes);
  min_bound = local_min_bound;
  max_bound = local_max_bound;
}

void OgreGismoMesh::prepareSurface(const std::shared_ptr<const iga_geometry_t>& igaGeo)
{
  using namespace Ogre;

  gismo::gsNormalField<real_t> normal_field{*igaGeo};

  const gismo::gsMatrix<real_t> param = igaGeo->parameterRange();
  // TODO: decide sample size according to some precise mathematical criteria.
  gismo::gsGridIterator<real_t, gismo::CUBE> pIter(param, 1000);

  const auto np = pIter.numPointsCwise();
  const auto npoints = np[0] * np[1];
  const auto ntriangles = 2 * (np[0]-1) * (np[1]-1);

  std::vector<float> positions_normals;
  std::vector<Ogre::uint16> triangles;

  auto local_min_bound = Vector3::ZERO;
  auto local_max_bound = Vector3::ZERO;

  positions_normals.reserve(vertexEntriesPerPoint() * npoints);
  triangles.reserve(2 * 3 * ntriangles);

  auto domain_points = pIter.toMatrix();
  // TODO: process in parallel.
  auto _positions  = igaGeo->eval(domain_points);
  auto _normals = normal_field.eval(domain_points);
  assert(_positions.cols() == npoints
         && "Wrong number of positions predicted.");
  assert(_positions.cols() == _normals.cols()
         && "We should have one normal for each vertex.");
  for(int i=0; i < _positions.cols(); ++i) {
    auto const& pcol = _positions.col(i);
    auto const& ncol = _normals.col(i);

    Vector3 pos(pcol[0], pcol[1], pcol[2]);
    Vector3 normal(ncol[0], ncol[1], ncol[2]);
    normal.normalise();

    // Sets the bounding box.
    local_min_bound.makeFloor(pos);
    local_max_bound.makeCeil(pos);

    // Sets the positions
    positions_normals.push_back(pos[0]);
    positions_normals.push_back(pos[1]);
    positions_normals.push_back(pos[2]);
    // Sets the normals
    positions_normals.push_back(normal[0]);
    positions_normals.push_back(normal[1]);
    positions_normals.push_back(normal[2]);
  }

  for(index_t j = 0; j < np[1]-1; ++j) {
    for(index_t i= 0; i < np[0]-1; ++i) {
      const index_t ind1 = j * np[0] + i;
      const index_t ind2 = ind1 + np[0];
      triangles.push_back(ind1);
      triangles.push_back(ind1+1);
      triangles.push_back(ind2+1);
      triangles.push_back(ind1);
      triangles.push_back(ind2+1);
      triangles.push_back(ind1+1);

      triangles.push_back(ind2+1);
      triangles.push_back(ind2);
      triangles.push_back(ind1);
      triangles.push_back(ind2+1);
      triangles.push_back(ind1);
      triangles.push_back(ind2);
    }
  }

  std::scoped_lock lock{mutex};
  vertex  = std::move(positions_normals);
  indexes = std::move(triangles);
  min_bound = local_min_bound;
  max_bound = local_max_bound;
}


void OgreGismoMesh::setVertexData()
{
  using namespace Ogre;

  assert(dimension && "The dimension was supposed to be set.");

  assert(mesh);

  assert(mesh->getSubMeshes().empty());
  mesh->createSubMesh();

  assert(!mesh->sharedVertexData);
  mesh->createVertexData();

  auto* vdata = mesh->sharedVertexData;
  auto* decl = vdata->vertexDeclaration;
  vdata->vertexCount = vertexEntriesPerPoint();

  assert(vblock_size == 0 && "VertexData can only be set once.");
  vblock_size += decl->addElement(0, vblock_size, VET_FLOAT3, VES_POSITION).getSize();
  if(dimension == 2) {
    vblock_size += decl->addElement(0, vblock_size, VET_FLOAT3, VES_NORMAL).getSize();
  }
}


void OgreGismoMesh::prepareHardwareBuffers()
{
  using namespace Ogre;

  // Vertexes.
  bool need_vertex_buffer = false;

  if(vertex_buffer_size <= 0) {
    need_vertex_buffer = true;
    vertex_buffer_size = 1;
  }

  auto needed_vertex_size = std::max(vertex.size(), vertexEntriesPerPoint());
  while(vertex_buffer_size < needed_vertex_size) {
    need_vertex_buffer = true;
    vertex_buffer_size *= 2;
  }
  while(vertex_buffer_size > 4*needed_vertex_size) {
    need_vertex_buffer = true;
    vertex_buffer_size /= 2;
  }

  if(need_vertex_buffer) {
    auto* vdata = mesh->sharedVertexData;
    auto* bind = vdata->vertexBufferBinding;

    vbuf = HardwareBufferManager::getSingleton().createVertexBuffer(
        vblock_size, vertex_buffer_size/vertexEntriesPerPoint(), HBU_GPU_ONLY);
    bind->setBinding(0, vbuf);
  }
  auto vbyte_count = vblock_size*vertex.size()/vertexEntriesPerPoint();
  assert(vbyte_count <= vbuf->getSizeInBytes());
  vbuf->writeData(0, sizeof(float)*vertex.size(), vertex.data(), true);


  // Indexes
  bool need_index_buffer = false;
  if(index_buffer_size <= 0) {
    need_index_buffer = true;
    index_buffer_size = 1;
  }

  auto needed_index_size = std::max(indexes.size(), (size_t)1);
  while(index_buffer_size < needed_index_size) {
    need_index_buffer = true;
    index_buffer_size *= 2;
  }

  while(index_buffer_size > 4*needed_index_size) {
    need_index_buffer = true;
    index_buffer_size /= 2;
  }

  if(need_index_buffer) {
    ibuf = HardwareBufferManager::getSingleton().createIndexBuffer(
        HardwareIndexBuffer::IT_16BIT, index_buffer_size, HBU_GPU_ONLY);
  }
  assert(sizeof(Ogre::uint16)*indexes.size() <= ibuf->getSizeInBytes());
  ibuf->writeData(0, sizeof(Ogre::uint16)*indexes.size(), indexes.data(), true);
}
