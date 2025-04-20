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

#ifndef DocumentTree_exceptions_H
#define DocumentTree_exceptions_H

#include <base/exceptions.h>
#include <base/expected_behaviour/SharedPtr.h>
#include <base/naming/Uuid.h>

namespace Naming
{
  class ExporterCommon;
}

namespace Document {
  class Container;
}

namespace Document::Exception
{

  class ElementAlreadyInContainer : public ::Exception::RunTimeError
  {
  public:
    ElementAlreadyInContainer(
        Naming::Uuid element_uuid, const Container& container);
    ElementAlreadyInContainer(
        const Naming::ExporterCommon& element, const Container& container);
  };

  class ElementNotInContainer : public ::Exception::RunTimeError
  {
  public:
    ElementNotInContainer(
        Naming::Uuid element_uuid, const Container& container);
    ElementNotInContainer(
        const Naming::ExporterCommon& element, const Container& container);
  };

}  // namespace Document

#endif
