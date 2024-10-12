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

#include <gismo/gsCore/gsFieldCreator.h>
#include <gismo/gsCore/gsGeometry.h>

#include <OGRE/OgreMesh.h>
#include <OGRE/OgreResource.h>

#include <memory>
#include <atomic>

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
  class OgreGismoMesh : public Ogre::ManualResourceLoader
  {
  public:
    OgreGismoMesh(std::shared_ptr<const iga_geometry_t> iga_geometry);
    void resetIgaGeometry(SharedPtr<const iga_geometry_t> iga_geometry);
    const SharedPtr<Ogre::Mesh>& getOgreMesh() const {return mesh;}

  protected:
    void prepareResource(Ogre::Resource* resource) override;
    void loadResource(Ogre::Resource* resource) override;

  private:
    SharedPtr<Ogre::Mesh> mesh;
    std::atomic<std::shared_ptr<const iga_geometry_t>> igaGeometry;

    // Prepared data.
    std::vector<float> vertex;
    std::vector<Ogre::uint16> indexes;
    Ogre::Vector3 min_bound;
    Ogre::Vector3 max_bound;

    int vertexEntriesPerPoint() const;

    void prepareCurve();
    void prepareSurface();

    void loadCurve();
    void loadSurface();
  };
}
