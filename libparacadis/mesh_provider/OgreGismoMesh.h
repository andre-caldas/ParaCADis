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
#include <base/geometric_primitives/DocumentGeometry.h>

#include <gismo/gismo.h>

#include <OGRE/OgreMesh.h>
#include <OGRE/OgreResource.h>

#include <memory>
#include <mutex>

namespace Mesh
{
  using native_geometry_t = Document::DocumentGeometry;
  using iga_geometry_t = native_geometry_t::iga_geometry_t;

  /**
   * A mesh for the IgA geometries provided by G+Smo.
   *
   * This class contains an Ogre::Mesh and a mesh loader that
   * converts the G+Smo geometry to the Ogre::Mesh.
   */
  class OgreGismoMesh
      : public Ogre::ManualResourceLoader
      , public std::enable_shared_from_this<OgreGismoMesh>
  {
  public:
    OgreGismoMesh(std::shared_ptr<const iga_geometry_t> iga_geometry);
    void init();

    void resetIgaGeometry(SharedPtr<const iga_geometry_t> iga_geometry);
    const SharedPtr<Ogre::Mesh>& getOgreMesh() const {return mesh;}

  protected:
    void justPrepare();
    void prepareResource(Ogre::Resource* resource) override;
    void loadResource(Ogre::Resource* resource) override;

  private:
    SharedPtr<Ogre::Mesh> mesh;
    std::atomic<std::shared_ptr<const iga_geometry_t>> igaGeometry;

    std::mutex mutex;

    // Prepared data.
    std::vector<float> vertex;
    std::vector<Ogre::uint16> indexes;
    Ogre::Vector3 min_bound;
    Ogre::Vector3 max_bound;

    // Buffers.
    int dimension = 0;
    size_t vblock_size = 0;
    size_t index_buffer_size = 0;
    size_t vertex_buffer_size = 0;

    Ogre::HardwareVertexBufferSharedPtr vbuf;
    Ogre::HardwareIndexBufferSharedPtr ibuf;

    size_t vertexEntriesPerPoint() const;

    void setVertexData();

    void prepareCurve(const std::shared_ptr<const iga_geometry_t>& igaGeo);
    void prepareSurface(const std::shared_ptr<const iga_geometry_t>& igaGeo);

    void prepareHardwareBuffers();
  };
}
