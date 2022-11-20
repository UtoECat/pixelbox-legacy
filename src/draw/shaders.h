/*
 * PixelBox
 * Copyright (C) 2022 UtoECat

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

// SHaders
static const char* vertex_shader_text =
"#version 330\n"
"layout(location = 0) in vec2 pos;\n"
"layout(location = 1) in vec2 wpos;\n"
"out vec2 rPos;\n"
"void main()\n"
"{\n"
"		gl_Position = vec4(pos, 0.0, 1.0);\n"
"		rPos = wpos;\n"
"}\n";
 
static const char* fragment_shader_text =
"#version 330\n"
"uniform sampler2D world;\n"
"uniform sampler2D colormap;\n"
"in vec2 rPos;\n"
"out vec4 color;\n"

"void main()\n"
"{\n"
"		color = texture(colormap, texture(world, rPos).rg) ;\n"
"}\n";
