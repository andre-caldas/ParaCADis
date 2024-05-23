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

#ifndef DocumentTree_PathCachePolicies_H
#define DocumentTree_PathCachePolicies_H

#include <base/expected_behaviour/SharedPtr.h>
#include <base/naming_scheme/path_cache_policies.h>

#include <vector>

namespace Document
{

  class TimedWeakChainWithTransforms : public TimedWeakChain
  {
  public:
    void prepare(token_iterator& tokens) override;
    const SharedPtr<ExporterBase>& topExporter() const override;
    void pushExporter(SharedPtr<ExporterBase>&& exporter) override;

  private:
    std::vector<WeakPtr<ExporterBase>> exporters;
    std::vector<NamingScheme::CoordinateSystem> transforms;
  };

}  // namespace NamingScheme

#endif
