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

#include "IgaProvider.h"
#include "OgreGismoMesh.h"

namespace Mesh
{
  class MeshProvider
  {
  public:
    static SharedPtr<MeshProvider>
    make_shared(SharedPtr<native_geometry_t> geometry,
                const SharedPtr<Threads::SignalQueue>& queue);

    static SharedPtr<MeshProvider>
    make_shared(SharedPtr<IgaProvider> provider,
                const SharedPtr<Threads::SignalQueue>& queue);

    const SharedPtr<Ogre::Mesh>& getOgreMesh() const
    {return mesh.getOgreMesh();}

  protected:
    MeshProvider(SharedPtr<IgaProvider> iga_provider);
    void slotUpdate();

    const SharedPtr<IgaProvider> igaProvider;
    OgreGismoMesh mesh;
  };
}
