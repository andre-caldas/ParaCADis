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

#include <concepts>

#include <gismo/gsCore/gsCurve.h>
#include <gismo/gsCore/gsSurface.h>

namespace Mesh
{
  template<typename GismoGeo>
  concept C_GismoCurve = requires(GismoGeo g)
  {
    typename GismoGeo::Scalar_t;
    {g} -> std::convertible_to<gismo::gsCurve<typename GismoGeo::Scalar_t>>;
  };

  template<typename GismoGeo>
  concept C_GismoSurface = requires(GismoGeo g)
  {
    typename GismoGeo::Scalar_t;
    {g} -> std::convertible_to<gismo::gsSurface<typename GismoGeo::Scalar_t>>;
  };

  template<typename Provider>
  concept C_ProvidesGismoCurve = requires(Provider s)
  {
    typename Provider::curve_t;
    {*(s.produceGismoCurve())} -> C_GismoCurve;
  };

  template<typename Provider>
  concept C_ProvidesGismoSurface = requires(Provider s)
  {
    typename Provider::surface_t;
    {*(s.produceGismoSurface())} -> C_GismoSurface;
  };
}
