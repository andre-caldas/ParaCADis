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

#include "lines.h"

Line2Points::Line2Points(Point start, Point end,
                         bool is_bounded_start, bool is_bounded_end)
    : Exporter{std::move(start), std::move(end), is_bounded_start, is_bounded_end}
{
}

std::unique_ptr<Line2Points> Line2Points::deepCopy() const
{
  auto gate = getReaderGate();
  return std::make_unique<Line2Points>(gate->start, gate->end,
                                       gate->is_bounded_start, gate->is_bounded_end);
}


LinePointDirection::LinePointDirection(Point start, Vector direction,
                                       bool is_bounded_start, bool is_bounded_end)
    : Exporter{std::move(start), std::move(direction), is_bounded_start, is_bounded_end}
{
}

std::unique_ptr<LinePointDirection> LinePointDirection::deepCopy() const
{
  auto gate = getReaderGate();
  return std::make_unique<LinePointDirection>(gate->start, gate->direction,
                                              gate->is_bounded_start, gate->is_bounded_end);
}

/**
 * Template instantiation.
 * There is no need to include NameSearchResult_impl.h for those.
 */
#include <base/naming_scheme/IExport.h>
#include <base/naming_scheme/IExport_impl.h>
#include <base/naming_scheme/NameSearchResult.h>
#include <base/naming_scheme/NameSearchResult_impl.h>

using namespace NamingScheme;

template class IExport<Line2Points>;
template class IExport<LinePointDirection>;

template class NameSearchResult<Line2Points>;
template class NameSearchResult<LinePointDirection>;
