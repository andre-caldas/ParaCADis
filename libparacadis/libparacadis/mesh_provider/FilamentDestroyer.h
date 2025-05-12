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

#pragma once

#include <libparacadis/base/expected_behaviour/SharedPtr.h>

namespace filament {
  class Engine;
}

namespace Mesh
{
  /**
   * In Filament, objects have to be destroyed through the Filament Engine.
   * Since destruction is thread-safe, we implement a wrapper for automatic
   * destruction.
   *
   * @attention
   * Objects members of classes must be declared in the reverse order they
   * need to be destroyed for the automatic destruction to happen in the
   * correct order.
   *
   * @attention
   * This class cannot be copied because the same object would be destroyed
   * twice. Use `SharedPtr<Destoyer<Filament::xxx>>` if you want to share the
   * destroyer.
   */
  template<typename T>
  class FilamentDestroyer
  {
  public:
    FilamentDestroyer() = delete;
    FilamentDestroyer(SharedPtr<filament::Engine> engine, T* data);
    ~FilamentDestroyer();

    FilamentDestroyer(const FilamentDestroyer&) = delete;
    FilamentDestroyer& operator=(const FilamentDestroyer&) = delete;

    FilamentDestroyer(FilamentDestroyer&&) = default;
    FilamentDestroyer& operator=(FilamentDestroyer&&) = default;

    T* get() { return data; }
    const T* get() const { return data; }

    T& operator*() { return *data; }
    const T& operator*() const { return *data; }

    T* operator->() { return data; }
    const T* operator->() const { return data; }

  private:
    T* data;
    SharedPtr<filament::Engine> engine;
  };
}
