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

#include <OGRE/OgreHardwareBufferManager.h>
#include <OGRE/OgreMeshManager.h>
#include <OGRE/OgreSubMesh.h>

#include <atomic>
#include <memory>
#include <format>

using namespace Mesh;
using namespace gismo;

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

  mesh->load();
}

void OgreGismoMesh::resetIgaGeometry(SharedPtr<const iga_geometry_t> iga_geometry)
{
  igaGeometry = std::move(iga_geometry.sliced());
  mesh->reload();
}


int OgreGismoMesh::vertexEntriesPerPoint() const
{
  auto igaGeo = igaGeometry.load();
  assert(igaGeo && "IgA geometry must be defined");
  return ((igaGeo->parDim() == 1) ? 3 : 6);
}

void OgreGismoMesh::prepareResource(Ogre::Resource*)
{
  auto igaGeo = igaGeometry.load();
  if(!igaGeo) {
    return;
  }

  if(igaGeo->parDim() == 1) {
    prepareCurve();
    return;
  }
  if(igaGeo->parDim() == 2) {
    prepareSurface();
    return;
  }
  assert(false && "Must be a curve or a surface.");
}

void OgreGismoMesh::loadResource(Ogre::Resource*)
{
  auto igaGeo = igaGeometry.load();
  if(!igaGeo) {
    return;
  }

  if(igaGeo->parDim() == 1) {
    loadCurve();
    return;
  }
  if(igaGeo->parDim() == 2) {
    loadSurface();
    return;
  }
  assert(false && "Must be a curve or a surface.");
}


void OgreGismoMesh::prepareCurve()
{
  using namespace Ogre;
  auto igaGeo = igaGeometry.load();
  if(!igaGeo) {
    return;
  }

  const gsMatrix<real_t> param = igaGeo->parameterRange();
  // TODO: decide sample size according to some precise mathematical criteria.
  gsGridIterator<real_t, CUBE> pIter(param, 100);

  const auto np = pIter.numPointsCwise();
  const auto npoints = np[0];

  auto& positions = vertex;

  positions.clear();
  indexes.clear();
  min_bound = Vector3::ZERO;
  max_bound = Vector3::ZERO;

  positions.reserve(vertexEntriesPerPoint() * npoints);
  indexes.reserve(npoints);

  auto domain_points = pIter.toMatrix();
  // TODO: process in parallel.
  auto _positions  = igaGeo->eval(domain_points);
  assert(_positions.cols() == npoints
         && "Wrong number of positions predicted.");
  for(int i=0; i < _positions.cols(); ++i) {
    auto const& pcol = _positions.col(i);

    Vector3 pos(pcol[0], pcol[1], pcol[2]);
    // Sets the bounding box.
    min_bound.makeFloor(pos);
    max_bound.makeCeil(pos);

    // Sets the positions
    positions.push_back(pos[0]);
    positions.push_back(pos[1]);
    positions.push_back(pos[2]);

    indexes.push_back(i);
  }
}

void OgreGismoMesh::prepareSurface()
{
  using namespace Ogre;
  auto igaGeo = igaGeometry.load();
  if(!igaGeo) {
    return;
  }

  gsNormalField<real_t> normal_field{*igaGeo};

  const gsMatrix<real_t> param = igaGeo->parameterRange();
  // TODO: decide sample size according to some precise mathematical criteria.
  gsGridIterator<real_t, CUBE> pIter(param, 1000);

  const auto np = pIter.numPointsCwise();
  const auto npoints = np[0] * np[1];
  const auto ntriangles = 2 * (np[0]-1) * (np[1]-1);

  auto& positions_normals = vertex;
  auto& triangles = indexes;

  positions_normals.clear();
  triangles.clear();
  min_bound = Vector3::ZERO;
  max_bound = Vector3::ZERO;

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
    min_bound.makeFloor(pos);
    max_bound.makeCeil(pos);

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
}


void OgreGismoMesh::loadCurve()
{
  assert(igaGeometry.load() && "Should not be called when IgA geometry is unset");

  using namespace Ogre;
  mesh->_setBounds(AxisAlignedBox(min_bound, max_bound));

  mesh->createVertexData();
  mesh->sharedVertexData->vertexCount = vertexEntriesPerPoint();
  auto* decl = mesh->sharedVertexData->vertexDeclaration;
  auto* bind = mesh->sharedVertexData->vertexBufferBinding;

  size_t offset = 0;
  offset += decl->addElement(0, offset, VET_FLOAT3, VES_POSITION).getSize();

  auto vbuf =
    HardwareBufferManager::getSingleton().createVertexBuffer(
      offset, vertex.size()/vertexEntriesPerPoint(), HBU_GPU_ONLY);
  vbuf->writeData(0, vbuf->getSizeInBytes(), vertex.data(), true);
  bind->setBinding(0, vbuf);

  auto ibuf =
    HardwareBufferManager::getSingleton().createIndexBuffer(
      HardwareIndexBuffer::IT_16BIT, indexes.size(), HBU_GPU_ONLY);
  ibuf->writeData(0, ibuf->getSizeInBytes(), indexes.data(), true);

  SubMesh* sub = mesh->createSubMesh();
  sub->operationType = RenderOperation::OT_LINE_STRIP;
//  sub->useSharedVertices = true;
  sub->indexData->indexBuffer = ibuf;
  sub->indexData->indexStart = 0;
  sub->indexData->indexCount = indexes.size();
}

void OgreGismoMesh::loadSurface()
{
  assert(igaGeometry.load() && "Should not be called when IgA geometry is unset");

  using namespace Ogre;
  mesh->_setBounds(AxisAlignedBox(min_bound, max_bound));

  mesh->createVertexData();
  mesh->sharedVertexData->vertexCount = vertexEntriesPerPoint();
  auto* decl = mesh->sharedVertexData->vertexDeclaration;
  auto* bind = mesh->sharedVertexData->vertexBufferBinding;

  size_t offset = 0;
  offset += decl->addElement(0, offset, VET_FLOAT3, VES_POSITION).getSize();
  offset += decl->addElement(0, offset, VET_FLOAT3, VES_NORMAL).getSize();

  auto vbuf =
    HardwareBufferManager::getSingleton().createVertexBuffer(
      offset, vertex.size()/vertexEntriesPerPoint(), HBU_GPU_ONLY);
  vbuf->writeData(0, vbuf->getSizeInBytes(), vertex.data(), true);
  bind->setBinding(0, vbuf);

  auto ibuf =
    HardwareBufferManager::getSingleton().createIndexBuffer(
      HardwareIndexBuffer::IT_16BIT, indexes.size(), HBU_GPU_ONLY);
  ibuf->writeData(0, ibuf->getSizeInBytes(), indexes.data(), true);

  SubMesh* sub = mesh->createSubMesh();
  sub->operationType = RenderOperation::OT_TRIANGLE_LIST;
//  sub->useSharedVertices = true;
  sub->indexData->indexBuffer = ibuf;
  sub->indexData->indexStart = 0;
  sub->indexData->indexCount = indexes.size();
}
