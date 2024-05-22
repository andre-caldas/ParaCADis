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

#include <catch2/catch_test_macros.hpp>

using namespace DocumentTree;

SCENARIO("Moving elements between containers", "[simple]")
{
  GIVEN("two containers 'a' and 'b' with a bunch of elements inside")
  {
    Real radius2{25};
    auto a        = SharedPtr<Container>::make_shared();
    auto b        = SharedPtr<Container>::make_shared();

    auto p        = SharedPtr<DeferenceablePoint>::make_shared(1, 2, 3);
    auto pw       = p.getWeakPtr();
    auto puuid    = p->getUuid();

    auto v        = SharedPtr<DeferenceableVector>::make_shared(4, 5, 6);
    auto vw       = v.getWeakPtr();
    auto vuuid    = v->getUuid();

    auto line     = SharedPtr<LinePointDirection>::make_shared(*p, *v);
    auto linew    = line.getWeakPtr();
    auto lineuuid = line->getUuid();

    auto circle   = SharedPtr<CirclePointRadius2Normal>::make_shared(*p, radius2, *v);
    auto circlew  = circle.getWeakPtr();
    auto ciruuid  = circle->getUuid();

    auto fixed    = SharedPtr<DeferenceablePoint>::make_shared(-1, -1, -1);
    auto fixedw   = fixed.getWeakPtr();
    auto fixuuid  = fixed->getUuid();

    a->addElement(p);
    p.reset();
    b->addElement(v);
    v.reset();
    a->addElement(line);
    line.reset();
    b->addElement(circle);
    circle.reset();
    a->addElement(fixed);
    fixed.reset();

    WHEN("we move from 'a' to 'b'")
    {
      a->moveElementTo(puuid, b);
      b->moveElementTo(vuuid, a);
      a->moveElementTo(lineuuid, b);
      b->moveElementTo(ciruuid, a);

      THEN("objects were actually moved")
      {
        REQUIRE(b->contains(puuid));
        REQUIRE(a->contains(vuuid));
        REQUIRE(b->contains(lineuuid));
        REQUIRE(a->contains(ciruuid));
        REQUIRE(a->contains(fixuuid));

        REQUIRE_FALSE(a->contains(puuid));
        REQUIRE_FALSE(b->contains(vuuid));
        REQUIRE_FALSE(a->contains(lineuuid));
        REQUIRE_FALSE(b->contains(ciruuid));
        REQUIRE_FALSE(b->contains(fixuuid));
      }

      THEN("objects were not destroyed")
      {
        REQUIRE(pw.lock());
        REQUIRE(vw.lock());
        REQUIRE(linew.lock());
        REQUIRE(circlew.lock());
        REQUIRE(fixedw.lock());
      }
    }
  }
}
