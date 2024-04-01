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

#include <catch2/catch_test_macros.hpp>

using namespace DocumentTree;

SCENARIO("Container basic manipulations", "[simple]")
{
  GIVEN("a Container")
  {
    Container a;
    auto      uuid = a.getUuid();
    REQUIRE(a.getUuid().isValid());
    THEN("name is unset and the uuid is valid")
    {
      REQUIRE(a.getName() == "");
      REQUIRE(a.getUuid() == uuid);
    }
    AND_WHEN("we set a name")
    {
      std::string name("container_name");
      a.setName(name);
      THEN("the name must be set")
      {
        REQUIRE(a.getName() == name);
        REQUIRE(a.getUuid() == uuid);
      }
      AND_WHEN("we set the name again")
      {
        std::string name("another name!!");
        a.setName(name);
        THEN("the name must be set")
        {
          REQUIRE(a.getName() == name);
          REQUIRE(a.getUuid() == uuid);
        }
        AND_WHEN("we reset it")
        {
          a.setName("");
          THEN("the name must be set")
          {
            REQUIRE(a.getName() == "");
            REQUIRE(a.getUuid() == uuid);
          }
        }
      }
    }
  }

  GIVEN("two instances of a Container")
  {
    Container a, b;
    auto      uuid_a = a.getUuid();
    auto      uuid_b = b.getUuid();

    REQUIRE(a.getUuid().isValid());
    REQUIRE(b.getUuid().isValid());
    THEN("Uuids must be different")
    {
      REQUIRE(a.getUuid() != b.getUuid());
    }
  }
}
