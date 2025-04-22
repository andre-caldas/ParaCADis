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

#include "exceptions.h"

#include "Container.h"

#include <libparacadis/base/naming/exceptions.h>

#include <format>

using namespace ::Exception;
using namespace Document;
using namespace Document::Exception;
using namespace Naming;

ElementAlreadyInContainer::ElementAlreadyInContainer(
    const ExporterCommon& element, const Container& container)
    : RunTimeError(std::format(
          "Container ({}) already has element ({}).", container.toString(),
          element.toString()))
{
}

ElementAlreadyInContainer::ElementAlreadyInContainer(
    Naming::Uuid element_uuid, const Container& container)
    : RunTimeError(std::format(
          "Container ({}) does not have element ({}).", container.toString(),
          element_uuid.toString()))
{
}

ElementNotInContainer::ElementNotInContainer(
    const ExporterCommon& element, const Container& container)
    : RunTimeError(std::format(
          "Container ({}) does not have element ({}).", container.toString(),
          element.toString()))
{
}

ElementNotInContainer::ElementNotInContainer(
    Naming::Uuid element_uuid, const Container& container)
    : RunTimeError(std::format(
          "Container ({}) does not have element ({}).", container.toString(),
          element_uuid.toString()))
{
}
