/*
 * PixelBox
 * World shaders implementation
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

#include "world.h"
#include <stdarg.h>
#include <stdio.h>

namespace pixelbox {

	static inline void errorf(int, const char* m, ...) {
		va_list args;
		va_start(args, m);
		vfprintf(stderr, m, args);
		va_end(args);
	}

	static void check_shader_error(GLuint shader) {
		char tmp[1024] = {0};
		int success = 1;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 1024, NULL, tmp);
			errorf(0, "Shader(%i) error : %s ", shader, tmp);
			throw "shader compilation error";
		}
	}

	static void check_program_error(GLuint prog) {
		char tmp[1024] = {0};
		int success = 1;
		glGetProgramiv(prog, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(prog, 1024, NULL, tmp);
			errorf(0, "Program linking error : %p", tmp);
			throw "program linking error";
		}
	}

	#include "shaders.h"

	void World::init_shader_program() {
		if (program == 0) {
			GLuint fragment = 0, vertex = 0;
			vertex = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex, 1, &vertex_shader_text, NULL);
			glCompileShader(vertex);
			check_shader_error(vertex);
			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &fragment_shader_text, NULL);
			glCompileShader(fragment);
			check_shader_error(fragment);
			program = glCreateProgram();
			if (!program) throw "Shader program cannot be created!";
			glAttachShader(program, vertex);
			glAttachShader(program, fragment);
			glLinkProgram(program);
			check_program_error(program);
			glDeleteShader(vertex);
			glDeleteShader(fragment);
		}
		glUseProgram(program);
	}

};
