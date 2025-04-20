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

using namespace Document;

SCENARIO("Removing elements from a container", "[simple]")
{
  GIVEN("a container and some objects inside it")
  {
    Real radius2{25};
    auto a        = SharedPtr<Container>::make_shared();

    auto p        = SharedPtr<DeferenceablePoint>::make_shared(1, 2, 3);
    auto pw       = p.getWeakPtr();
    auto puuid    = p->getUuid();

    auto v        = SharedPtr<DeferenceableVector>::make_shared(4, 5, 6);
    auto vw       = v.getWeakPtr();
    auto vuuid    = v->getUuid();

    auto line     = SharedPtr<LinePointDirection>::make_shared(*p, *v);
    auto linew    = line.getWeakPtr();
    auto circle   = SharedPtr<CirclePointRadius2Normal>::make_shared(*p, radius2, *v);
    auto circlew  = circle.getWeakPtr();
    auto outside  = SharedPtr<DeferenceablePoint>::make_shared(-1, -1, -1);
    auto outsidew = outside.getWeakPtr();

    a->addElement(p);
    p.reset();
    a->addElement(v);
    v.reset();
    a->addElement(line);
    line.reset();
    a->addElement(circle);
    circle.reset();
    outside.reset();

    THEN("Objects inside shall not be destroyed when released outside")
    {
      REQUIRE(pw.lock());
      REQUIRE(vw.lock());
      REQUIRE(linew.lock());
      REQUIRE(circlew.lock());
      REQUIRE_FALSE(outsidew.lock());

      AND_WHEN("we remove the last reference to an object")
      {
        a->removeElement(puuid); // Using uuid.
        a->removeElement(linew.lock());

        THEN("Objects inside shall not be destroyed when released outside")
        {
          // Notice that p was simply copied to circle.
          // The circle does not hold a SharedPtr to p.
          REQUIRE_FALSE(pw.lock());
          REQUIRE(vw.lock());
          REQUIRE_FALSE(linew.lock());
          REQUIRE(circlew.lock());
        }
      }
    }
  }

  GIVEN("a nested container with some objects inside it")
  {
    Real radius2{25};
    auto parent   = SharedPtr<Container>::make_shared();

    auto xboth    = SharedPtr<DeferenceablePoint>::make_shared(7, 8, 9);
    auto xbothw   = xboth.getWeakPtr();
    auto xuuid    = xboth->getUuid();

    auto a        = SharedPtr<Container>::make_shared();
    auto aw       = a.getWeakPtr();
    auto auuid    = a->getUuid();

    auto p        = SharedPtr<DeferenceablePoint>::make_shared(1, 2, 3);
    auto pw       = p.getWeakPtr();
    auto puuid    = p->getUuid();

    auto v        = SharedPtr<DeferenceableVector>::make_shared(4, 5, 6);
    auto vw       = v.getWeakPtr();
    auto vuuid    = v->getUuid();

    auto line     = SharedPtr<LinePointDirection>::make_shared(*p, *v);
    auto linew    = line.getWeakPtr();
    auto circle   = SharedPtr<CirclePointRadius2Normal>::make_shared(*p, radius2, *v);
    auto circlew  = circle.getWeakPtr();
    auto outside  = SharedPtr<DeferenceablePoint>::make_shared(-1, -1, -1);
    auto outsidew = outside.getWeakPtr();
    auto outuuid  = outside->getUuid();

    a->addElement(xboth);
    a->addElement(p);
    p.reset();
    a->addElement(v);
    v.reset();
    a->addElement(line);
    line.reset();
    a->addElement(circle);
    circle.reset();

    parent->addElement(xboth);
    xboth.reset();
    parent->addElement(outside);
    outside.reset();
    parent->addElement(a);
    a.reset();

    WHEN("we remove the inner container")
    {
      parent->removeElement(auuid);

      THEN("Objects only in 'a' shall have been destroyed")
      {
        REQUIRE_FALSE(aw.lock()); // 'a' shall have been destroyed.
        REQUIRE_FALSE(pw.lock());
        REQUIRE_FALSE(vw.lock());
        REQUIRE_FALSE(linew.lock());
        REQUIRE_FALSE(circlew.lock());
        REQUIRE(outsidew.lock());
        REQUIRE(xbothw.lock());
      }
    }
  }
}
