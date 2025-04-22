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

using namespace Document;

Line2Points::Line2Points(Point start, Point end,
                         bool is_bounded_start, bool is_bounded_end)
    : Exporter(std::move(start), std::move(end), is_bounded_start, is_bounded_end)
{
}

SharedPtr<Line2Points> Line2Points::deepCopy() const
{
  Threads::ReaderGate gate{*this};
  return SharedPtrWrap<Line2Points>(gate->start, gate->end,
                                    gate->is_bounded_start, gate->is_bounded_end);
}

SharedPtr<const DocumentGeometry::iga_curve_t> Line2Points::produceIgaCurve() const
{
  assert(false && "Implement!");
  return {};
}


LinePointDirection::LinePointDirection(Point start, Vector direction,
                                       bool is_bounded_start, bool is_bounded_end)
    : Exporter(std::move(start), std::move(direction), is_bounded_start, is_bounded_end)
{
}

SharedPtr<LinePointDirection> LinePointDirection::deepCopy() const
{
  Threads::ReaderGate gate{*this};
  return SharedPtrWrap<LinePointDirection>(gate->start, gate->direction,
                                              gate->is_bounded_start, gate->is_bounded_end);
}

SharedPtr<const DocumentGeometry::iga_curve_t> LinePointDirection::produceIgaCurve() const
{
  assert(false && "Implement!");
  return {};
}

/**
 * Template instantiation.
 * There is no need to include NameSearch_impl.h for those.
 */
#include <libparacadis/base/naming/IExport.h>
#include <libparacadis/base/naming/NameSearch.h>

template class Naming::IExport<Line2Points>;
template class Naming::IExport<LinePointDirection>;

template class Naming::NameSearch<Line2Points>;
template class Naming::NameSearch<LinePointDirection>;
