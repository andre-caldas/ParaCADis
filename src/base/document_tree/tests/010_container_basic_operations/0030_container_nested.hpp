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

SCENARIO("Nested containers", "[simple]")
{
  GIVEN("some containers")
  {
    auto a = SharedPtr<Container>::make_shared();
    auto b = SharedPtr<Container>::make_shared();
    auto c = SharedPtr<Container>::make_shared();
    auto d = SharedPtr<Container>::make_shared();
    WHEN("we nest the containers")
    {
      a->addElement(b); // As a container
      b->addElement(c.cast<NamingScheme::ExporterBase>());   // As an element
      c->addElement(d);
      THEN("they actually belong to their 'parent' container")
      {
        REQUIRE(a->contains(b));
        REQUIRE(b->contains(c));
        REQUIRE(c->contains(d));

        REQUIRE_FALSE(a->contains(a));
        REQUIRE_FALSE(a->contains(c));
        REQUIRE_FALSE(a->contains(d));

        REQUIRE_FALSE(b->contains(a));
        REQUIRE_FALSE(b->contains(b));
        REQUIRE_FALSE(b->contains(d));

        REQUIRE_FALSE(c->contains(a));
        REQUIRE_FALSE(c->contains(b));
        REQUIRE_FALSE(c->contains(c));

        REQUIRE_FALSE(d->contains(a));
        REQUIRE_FALSE(d->contains(b));
        REQUIRE_FALSE(d->contains(c));
        REQUIRE_FALSE(d->contains(d));
      }
      THEN("even when they are disguised as 'ExporterBase'")
      {
        REQUIRE(a->contains(static_cast<ExporterBase&>(*b)));
        REQUIRE(b->contains(static_cast<ExporterBase&>(*c)));
        REQUIRE(c->contains(static_cast<ExporterBase&>(*d)));

        REQUIRE_FALSE(a->contains(static_cast<ExporterBase&>(*a)));
        REQUIRE_FALSE(a->contains(static_cast<ExporterBase&>(*c)));
        REQUIRE_FALSE(a->contains(static_cast<ExporterBase&>(*d)));
      }
      AND_WHEN("we nest the same container in two places")
      {
        a->addElement(c);
        a->addElement(d);
        THEN("even when they are disguised as 'ExporterBase'")
        {
          REQUIRE(a->contains(b));
          REQUIRE(a->contains(c));
          REQUIRE(a->contains(d));

          REQUIRE_FALSE(a->contains(a));

          REQUIRE_FALSE(b->contains(a));
          REQUIRE_FALSE(b->contains(d));

          REQUIRE_FALSE(d->contains(a));
          REQUIRE_FALSE(d->contains(b));
          REQUIRE_FALSE(d->contains(c));
          REQUIRE_FALSE(d->contains(d));
        }
        AND_WHEN("even when we introduce loops")
        {
          d->addElement(a);
          d->addElement(b);
          d->addElement(c);
          d->addElement(d);
          THEN("loops are introduced normally")
          {
            REQUIRE(a->contains(b));
            REQUIRE(b->contains(c));
            REQUIRE(c->contains(d));

            REQUIRE(d->contains(a));
            REQUIRE(d->contains(b));
            REQUIRE(d->contains(c));
            REQUIRE(d->contains(d));

            REQUIRE_FALSE(b->contains(a));
            REQUIRE_FALSE(b->contains(b));
            REQUIRE_FALSE(b->contains(d));
          }
        }
      }
    }
  }
}
