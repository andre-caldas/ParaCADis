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

#pragma once

#include <base/threads/message_queue/SignalQueue.h>

union SDL_Event;
struct SDL_MouseButtonEvent;

struct SDL_TextEditingEvent;
struct SDL_TextInputEvent;
struct SDL_KeyboardEvent;

struct SDL_JoyButtonEvent;
struct SDL_JoyAxisEvent;
struct SDL_JoyHatEvent;

namespace SceneGraph
{

    class GraphicsSystem;
    class CameraController;

    class UiState
    {
    protected:
        GraphicsSystem& graphics;
        CameraController& camera_controller;

    public:
        UiState();
        ~UiState() override;

        void keyPressed( const SDL_KeyboardEvent &arg );
        void keyReleased( const SDL_KeyboardEvent &arg );

        void mouseMoved( const SDL_Event &arg );
    };
}  // namespace Demo

#endif
