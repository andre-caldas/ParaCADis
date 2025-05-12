// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2025 André Caldas <andre.em.caldas@gmail.com>            *
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

#include "FilamentDestroyer.h"

#include <filament/Engine.h>
#include <filament/Renderer.h>
#include <filament/SwapChain.h>
#include <filament/Scene.h>
#include <filament/View.h>
#include <filament/VertexBuffer.h>
#include <filament/IndexBuffer.h>

namespace Mesh
{
  template<typename T>
  FilamentDestroyer<T>::FilamentDestroyer(SharedPtr<filament::Engine> _engine, T* _data)
    : engine(std::move(_engine))
    , data(_data)
  {
  }

  template<typename T>
  FilamentDestroyer<T>::~FilamentDestroyer()
  {
    engine->destroy(data);
  }
}

template class Mesh::FilamentDestroyer<filament::Renderer>;
template class Mesh::FilamentDestroyer<filament::SwapChain>;
template class Mesh::FilamentDestroyer<filament::Scene>;
template class Mesh::FilamentDestroyer<filament::View>;
template class Mesh::FilamentDestroyer<filament::VertexBuffer>;
template class Mesh::FilamentDestroyer<filament::IndexBuffer>;
