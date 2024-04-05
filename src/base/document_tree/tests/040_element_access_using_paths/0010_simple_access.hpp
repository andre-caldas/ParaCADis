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

SCENARIO("Accessing a container element using a path", "[simple]")
{
  GIVEN("a container and some objects")
  {
    Real radius{5};
    auto a     = SharedPtr<Container>::make_shared();

    auto p     = SharedPtr<DeferenceablePoint>::make_shared(1, 2, 3);
    auto pw    = p.getWeakPtr();
    auto puuid = p->getUuid();

    auto v     = SharedPtr<DeferenceableVector>::make_shared(4, 5, 6);
    auto vw    = v.getWeakPtr();
    auto vuuid = v->getUuid();

    auto l     = SharedPtr<LinePointDirection>::make_shared(*p, *v);
    auto lw    = l.getWeakPtr();
    auto c     = SharedPtr<CirclePointRadiusNormal>::make_shared(*p, radius, *v);
    auto cw    = c.getWeakPtr();

    WHEN("we add them to the container")
    {
      a->addElement(p);
      auto pname = "This is p!";
      p->setName(pname);
      p.reset();

      a->addElement(v);
      auto vname = "This is v!";
      v->setName(vname);
      v.reset();

      a->addElement(l);
      auto lname = "This is a line!";
      l->setName(lname);
      l.reset();

      a->addElement(c);
      auto cname = "This is a circle!";
      c->setName(cname);
      c.reset();

      AND_WHEN("we set up a reference")
      {
        ReferenceTo<DeferenceablePoint>  p_ref{a, pname};
        ReferenceTo<DeferenceableVector> v_ref{a, vname};
        ReferenceTo<LinePointDirection>  l_ref{a, lname};
        ReferenceTo<DeferenceablePoint>  c_ref{a, cname};

        THEN("we get a working reference")
        {
          REQUIRE(pw.lock() == p_ref.resolve());
          REQUIRE(vw.lock() == v_ref.resolve());
          REQUIRE(lw.lock() == l_ref.resolve());
          REQUIRE(cw.lock() == c_ref.resolve());
        }
      }
    }
  }
}
