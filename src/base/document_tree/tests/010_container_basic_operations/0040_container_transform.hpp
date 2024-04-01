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

#include <base/document_tree/Container.h>
#include <base/geometric_primitives/CoordinateSystem.h>
#include <base/naming_scheme/ReferenceToObject.h>

#include <catch2/catch_test_macros.hpp>

using namespace DocumentTree;

SCENARIO("Container affine transformations", "[simple]")
{
  GIVEN("A container with objects inside it")
  {
    Real radius{5};
    auto a = SharedPtr<Container>::make_shared();
    auto p       = SharedPtr<DeferenceablePoint>::make_shared(1, 2, 3);
    auto v       = SharedPtr<DeferenceableVector>::make_shared(1, 2, 3);
    auto line    = SharedPtr<LinePointDirection>::make_shared(*p, *v);
    auto circle  = SharedPtr<CirclePointRadiusNormal>::make_shared(*p, radius, *v);

    auto p_out      = p;
    auto v_out      = v;
    auto line_out   = line->deep_copy();
    auto circle_out = circle->deep_copy();

    a->addElement(p);
    a->addElement(v);
    a->addElement(line);
    a->addElement(circle);

    WHEN("we transform the container")
    {
      Point  origin = {-3, 1, 2};
      Vector x      = {-1, 7, 9};
      Vector y      = {7, 1, 9};
      int    z      = -1;
      CoordinateSystem T{origin, x, y, z};

      THEN("they get properly transformed")
      {
        assert(false);
      }

      THEN("the original objects are intact")
      {
        assert(false);
      }

      AND_WHEN("we nest containers and transforms")
      {
        assert(false);

        THEN("they get transformed in chain")
        {
          assert(false);
        }
      }
    }
    }
  }
}
