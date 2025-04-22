// SPDX-License-Identifier: GPL-3.0-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2025 André Caldas <andre.em.caldas@gmail.com>            *
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

#pragma once

#include <libparacadis/base/data_description/Description.h>
#include <libparacadis/base/expected_behaviour/SharedPtr.h>

namespace ParacadisImGui
{
  template<typename T>
  class ImGuiDialog
  {
    using description_t = DataDescription::Description<T>;

  public:
    ImGuiDialog(SharedPtr<T> _data)
      : data(std::move(_data))
      , description(*data)
    {}

    bool operator()();

  private:
    SharedPtr<T> data;
    description_t description;
  };
}

#include"ImGuiDialog.hpp"
