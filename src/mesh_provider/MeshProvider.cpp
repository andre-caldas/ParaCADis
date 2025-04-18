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

#include "MeshProvider.h"

#include "OgreGismoMesh.h"

#include <memory>

using namespace Mesh;

/*
 * MeshProvider
 */
MeshProvider::MeshProvider(SharedPtr<IgaProvider> iga_provider)
    : igaProvider(std::move(iga_provider))
    , mesh(igaProvider->getIgaGeometry())
{}

SharedPtr<MeshProvider>
MeshProvider::make_shared(SharedPtr<IgaProvider> iga_provider,
                          const SharedPtr<Threads::SignalQueue>& queue)
{
  auto self = SharedPtr<MeshProvider>::from_pointer(new MeshProvider(iga_provider));
  iga_provider->igaChangedSig.connect(
      std::move(iga_provider), queue, self, &MeshProvider::slotUpdate);
  return self;
}

void MeshProvider::slotUpdate()
{
  mesh->resetIgaGeometry(igaProvider->getIgaGeometry());
}
