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
#include <base/geometric_primitives/circles.h>
#include <base/geometric_primitives/deferenceables.h>
#include <base/geometric_primitives/lines.h>
#include <base/naming_scheme/Exporter.h>

#include <catch2/catch_test_macros.hpp>

using namespace DocumentTree;
using namespace NamingScheme;

SCENARIO("Populate a with simple objects", "[simple]")
{
  GIVEN("a Container and some geometric objects")
  {
    Container a;
    auto      p       = Exporter::make_shared<DeferenceablePoint>(1, 2, 3);
    auto      v       = Exporter::make_shared<DeferenceableVector>(4, 5, 6);
    auto      line    = Exporter::make_shared<LinePointDirection>(p, v);
    auto      circle  = Exporter::make_shared<CirclePointRadiusNormal>(p, v);
    auto      outside = Exporter::make_shared<DeferenceablePoint>(-1, -1, -1);

    WHEN("we add them to the a")
    {
      a.add(p);
      a.add(v);
      a.add(line);
      a.add(circle);

      THEN("they do belong to the container")
      {
        REQUIRE(a.contains(p));
        REQUIRE(a.contains(v));
        REQUIRE(a.contains(line));
        REQUIRE(a.contains(circle));
        REQUIRE_FALSE(a.contains(outside));
      }
      THEN("we can search by their uuid")
      {
        REQUIRE(a.contains(p->getUuid()));
        REQUIRE(a.contains(v->getUuid()));
        REQUIRE(a.contains(line->getUuid()));
        REQUIRE(a.contains(circle->getUuid()));
        REQUIRE_FALSE(a.contains(outside->getUuid()));
      }
      AND_WHEN("we set a name")
      {
        line->setName("a line name");
        circle->setName("A difiçũlt ñâmé");
        outside->setName("outside is not inside! :-)");

        THEN("we can find them using their name")
        {
          REQUIRE_FALSE(a.contains("abc"));

          REQUIRE(a.contains(line->getName()));
          REQUIRE(a.contains(circle->getName()));
          REQUIRE(a.contains(circle->getName() + ""));

          REQUIRE_FALSE(a.contains(circle->getName() + " "));
          REQUIRE_FALSE(a.contains(outside->getName()));
        }
      }
    }
  }
}
