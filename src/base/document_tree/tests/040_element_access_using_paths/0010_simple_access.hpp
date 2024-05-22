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

SCENARIO("Accessing a container element using a path", "[simple]")
{
  GIVEN("a container and some objects")
  {
    Real radius2{25};
    Real px{1};
    Real py{2};
    Real pz{3};
    Real vx{4};
    Real vy{5};
    Real vz{6};
    auto a     = SharedPtr<Container>::make_shared();

    auto p     = SharedPtr<DeferenceablePoint>::make_shared(px, py, pz);
    auto pw    = p.getWeakPtr();
    auto puuid = p->getUuid();

    auto v     = SharedPtr<DeferenceableVector>::make_shared(vx, vy, vz);
    auto vw    = v.getWeakPtr();
    auto vuuid = v->getUuid();

    auto l     = SharedPtr<LinePointDirection>::make_shared(*p, *v);
    auto lw    = l.getWeakPtr();
    auto c     = SharedPtr<CirclePointRadius2Normal>::make_shared(*p, radius2, *v);
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
        NamingScheme::ReferenceTo<DeferenceablePoint>      p_ref{a, pname};
        NamingScheme::ReferenceTo<DeferenceableVector>     v_ref{a, vname};
        NamingScheme::ReferenceTo<LinePointDirection>      l_ref{a, lname};
        NamingScheme::ReferenceTo<CirclePointRadius2Normal> c_ref{a, cname};

        THEN("we get a working reference")
        {
          REQUIRE(pw.lock().get() == &*(ReaderGateKeeper{p_ref.resolve()}));
          REQUIRE(vw.lock().get() == &*(ReaderGateKeeper{v_ref.resolve()}));
          REQUIRE(lw.lock().get() == &*(ReaderGateKeeper{l_ref.resolve()}));
          REQUIRE(cw.lock().get() == &*(ReaderGateKeeper{c_ref.resolve()}));

          AND_WHEN("we set up an even further reference")
          {
            NamingScheme::ReferenceTo<Real> px_ref{a, pname, "x"};
            NamingScheme::ReferenceTo<Real> py_ref{a, pname, "y"};
            NamingScheme::ReferenceTo<Real> pz_ref{a, pname, "z"};
            NamingScheme::ReferenceTo<Real> pbad_ref{a, pname, "x bad"};

            NamingScheme::ReferenceTo<Real> vx_ref{a, vname, "x"};
            NamingScheme::ReferenceTo<Real> vy_ref{a, vname, "y"};
            NamingScheme::ReferenceTo<Real> vz_ref{a, vname, "z"};
            NamingScheme::ReferenceTo<Real> vbad_ref{a, vname, "y bad"};

            NamingScheme::ReferenceTo<Real> lsx_ref{a, lname, "start", "x"};
            NamingScheme::ReferenceTo<Real> lsy_ref{a, lname, "start", "y"};
            NamingScheme::ReferenceTo<Real> lsz_ref{a, lname, "start", "z"};
            NamingScheme::ReferenceTo<Real> ldx_ref{a, lname, "direction", "x"};
            NamingScheme::ReferenceTo<Real> lbad_ref{a, vname, "start"};

            NamingScheme::ReferenceTo<Real> ccx_ref{a, cname, "center", "x"};
            NamingScheme::ReferenceTo<Real> cnx_ref{a, cname, "normal", "x"};
            NamingScheme::ReferenceTo<Real>  cr_ref{a, cname, "radius2"};
            NamingScheme::ReferenceTo<DeferenceablePoint>
                                           cbad_ref{a, lname, "center", "x"};

            THEN("we get working references")
            {
              REQUIRE(px == *(ReaderGateKeeper{px_ref.resolve()}));
              REQUIRE(py == *(ReaderGateKeeper{py_ref.resolve()}));
              REQUIRE(pz == *(ReaderGateKeeper{pz_ref.resolve()}));

              REQUIRE(vx == *(ReaderGateKeeper{vx_ref.resolve()}));
              REQUIRE(vy == *(ReaderGateKeeper{vy_ref.resolve()}));
              REQUIRE(vz == *(ReaderGateKeeper{vz_ref.resolve()}));

              REQUIRE(px == *(ReaderGateKeeper{lsx_ref.resolve()}));
              REQUIRE(py == *(ReaderGateKeeper{lsy_ref.resolve()}));
              REQUIRE(pz == *(ReaderGateKeeper{lsz_ref.resolve()}));
              REQUIRE(vx == *(ReaderGateKeeper{ldx_ref.resolve()}));

              REQUIRE(px == *(ReaderGateKeeper{ccx_ref.resolve()}));
              REQUIRE(vx == *(ReaderGateKeeper{cnx_ref.resolve()}));
              REQUIRE(radius2 == *(ReaderGateKeeper{cr_ref.resolve()}));
            }

            THEN("bad references evaluate to false")
            {
              REQUIRE_FALSE(pbad_ref.resolve());
              REQUIRE_FALSE(vbad_ref.resolve());
              REQUIRE_FALSE(lbad_ref.resolve());
              REQUIRE_FALSE(cbad_ref.resolve());
            }

            THEN("bad references throw when accessed")
            {
              REQUIRE_THROWS_AS(*(ReaderGateKeeper{pbad_ref.resolve()}),
                                std::bad_optional_access);
              REQUIRE_THROWS_AS(*(ReaderGateKeeper{vbad_ref.resolve()}),
                                std::bad_optional_access);
              REQUIRE_THROWS_AS(*(ReaderGateKeeper{lbad_ref.resolve()}),
                                std::bad_optional_access);
              REQUIRE_THROWS_AS(*(ReaderGateKeeper{cbad_ref.resolve()}),
                                std::bad_optional_access);
            }
          }
        }
      }
    }
  }
}
