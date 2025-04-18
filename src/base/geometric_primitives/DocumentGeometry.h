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
#include <base/naming/Exporter.h>
#include <base/threads/message_queue/Signal.h>

#include <gismo/gismo.h>

#include <concepts>

namespace Document
{
  class DocumentGeometry
    : public virtual Naming::ExporterCommon
  {
  public:
    using iga_geometry_t = gismo::gsGeometry<real_t>;
    using iga_curve_t    = gismo::gsCurve<real_t>;
    using iga_surface_t  = gismo::gsSurface<real_t>;

    virtual SharedPtr<const iga_geometry_t> getIgaGeometry() const = 0;
    virtual ~DocumentGeometry() = default;
  };


  class DocumentCurve : public DocumentGeometry
  {
  public:
    SharedPtr<const iga_geometry_t> getIgaGeometry() const override;
    SharedPtr<const iga_curve_t> getIgaCurve() const;

  protected:
    mutable std::atomic<std::shared_ptr<const iga_curve_t>> gismoGeometry;
    virtual SharedPtr<const iga_curve_t> produceIgaCurve() const = 0;
  };


  class DocumentSurface : public DocumentGeometry
  {
  public:
    SharedPtr<const iga_geometry_t> getIgaGeometry() const override;
    SharedPtr<const iga_surface_t> getIgaSurface() const;

  protected:
    mutable std::atomic<std::shared_ptr<const iga_surface_t>> gismoGeometry;
    virtual SharedPtr<const iga_surface_t> produceIgaSurface() const = 0;
  };


  template<typename T>
  concept C_IsDocumentGeometry = std::derived_from<T, DocumentGeometry>;
}
