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

/**
 * @file
 * A IgaProvider is a class that reads a native geometric object
 * and generates a mesh that can be processed using Isogeometric Analysis.
 *
 * Iga providers keep a reference to a native object of type DocumentGeometry
 * and a newly generated G+Smo object.
 *
 * Therefore, we implement two auxiliary classes to handle those references:
 * IgaGeometryHolder and NativeGeometryHolder.
 */

#pragma once

#include <libparacadis/base/geometric_primitives/DocumentGeometry.h>

#include <atomic>
#include <memory>

namespace Mesh
{
  using native_geometry_t = Document::DocumentGeometry;
  using iga_geometry_t = native_geometry_t::iga_geometry_t;

  /**
   * Holds (shared_ptr to) a constant IgA geometry and emits a signal
   * when the shared_ptr changes.
   */
  class IgaGeometryHolder
  {
  public:
    virtual ~IgaGeometryHolder() = default;

    std::shared_ptr<const iga_geometry_t> getIgaGeometry() const
    {return igaGeometry.load();}

    /**
     * Signals that the IgA data structure was changed and is ready.
     */
    Threads::Signal<> igaChangedSig;

  protected:
    void setIgaGeometry(std::shared_ptr<const iga_geometry_t> value);

    std::atomic<std::shared_ptr<const iga_geometry_t>> igaGeometry;
  };


  /**
   * Sets up an IgA geometry that is updated when the original
   * native geometry changes.
   */
  class IgaProvider
      : public IgaGeometryHolder
  {
  public:
    static SharedPtr<IgaProvider>
    make_shared(SharedPtr<native_geometry_t> geometry,
                const SharedPtr<Threads::SignalQueue>& queue);

  protected:
    IgaProvider(SharedPtr<native_geometry_t> geometry);

    WeakPtr<native_geometry_t> geometryWeak;

    /**
     * Called whenever DeferenceableGeometry changes.
     */
    void slotUpdate();
  };
}
