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
"layout(location = 1) in vec2 texpos;\n"
"out vec2 rPos;\n"
"void main()\n"
"{\n"
"		gl_Position = vec4(pos.x, pos.y * -1, 0.0, 1.0);\n"
"		rPos = texpos;\n"
"}\n";
 
static const char* fragment_shader_text =
"#version 330\n"
"uniform sampler2D chunk;\n"
"in  vec2 rPos;\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"\n"
"		vec4 val = texture(chunk, rPos);\n"
"		int Ftype = int(val.r * 255);\n"
"		int type = Ftype & 63;\n"
"		int kind = Ftype >> 6;\n"
"		float r  = (type & 3) + (kind == 1 ? 1 : 0);\n"
"		float g  = ((type >> 2) & 3) + (kind == 2 ? 1 : 0);\n"
"		float b  = ((type >> 4) & 3) + (kind == 3 ? 1 : 0);\n"
"		color = vec4(r/4.0, g/4.0, b/4.0, 1);\n"
"}\n";
