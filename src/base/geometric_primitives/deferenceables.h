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

#include "types.h"

#include <base/expected_behaviour/SharedPtrWrap.h>
#include <base/naming_scheme/Exporter.h>
#include <base/naming_scheme/IExport.h>

/**
 * DataStruct for DeferenceablePoint and DeferenceableVector.
 */
struct TripletStruct {
  Real x = 0;
  Real y = 0;
  Real z = 0;
};

/**
 * A point that exports its coordinates.
 *
 * This is not a native point. It is to be used only when you need a point
 * that exports its coordinates. In particular, if it is a geometric object
 * to be placed in the DocumenTree, use this and not the kernel native Point.
 *
 * @todo Maybe we should warrant we use a CGAL kernel that may have its
 * coordinates directly accessed and simply use it.
 */
class DeferenceablePoint
    : public NamingScheme::Exporter<TripletStruct>
    , public NamingScheme::IExportStruct<Real, TripletStruct,
                                       {&TripletStruct::x, "x"},
                                       {&TripletStruct::y, "y"},
                                       {&TripletStruct::z, "z"}>
{
  ONLY_SHAREDPTRWRAP()
  DeferenceablePoint() = default;
  DeferenceablePoint(DeferenceablePoint&&) = default;
  DeferenceablePoint(const Point& p);
  DeferenceablePoint(Real x, Real y, Real z);
public:
  operator Point() const noexcept;

  SharedPtr<DeferenceablePoint> deepCopy() const;
  SharedPtr<NamingScheme::ExporterBase> deepCopyExporter() const override
  { return deepCopy(); }
};


/**
 * A vector that exports its coordinates.
 *
 * This is not a native vector. It is to be used only when you need a vector
 * that exports its coordinates. In particular, if it is a geometric object
 * to be placed in the DocumenTree, use this and not the kernel native Point.
 *
 * @todo Maybe we should warrant we use a CGAL kernel that may have its
 * coordinates directly accessed and simply use it.
 */
class DeferenceableVector
    : public NamingScheme::Exporter<TripletStruct>
    , public NamingScheme::IExportStruct<Real, TripletStruct,
                                       {&TripletStruct::x, "x"},
                                       {&TripletStruct::y, "y"},
                                       {&TripletStruct::z, "z"}>
{
  ONLY_SHAREDPTRWRAP()
  DeferenceableVector() = default;
  DeferenceableVector(DeferenceableVector&&) = default;
  DeferenceableVector(const Vector& v);
  DeferenceableVector(Real x, Real y, Real z);
public:
  operator Vector() const noexcept;

  SharedPtr<DeferenceableVector> deepCopy() const;
  SharedPtr<NamingScheme::ExporterBase> deepCopyExporter() const override
  { return deepCopy(); }

//  std::string toString() const override;
};
