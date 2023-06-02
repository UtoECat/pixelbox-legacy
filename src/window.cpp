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

#include "window.h"

namespace pixelbox {

	static void errcb	(int, const char* c) {
		fprintf(stderr, "GLFW Error %s!", c);
	}

	static inline void initglfw() {
		glfwSetErrorCallback(errcb);
		glfwInit();
	}

	Window::Window() {
		initglfw();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GALOGEN_API_VER_MAJ);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
		W = glfwCreateWindow(640, 480, "PixelBox", NULL, NULL);
		if (!W) throw "can't open Window!";
		glfwSetFramebufferSizeCallback(W, resizecb);
		glfwSetWindowSizeLimits(W, 640, 480, GLFW_DONT_CARE, GLFW_DONT_CARE);
		glfwSetWindowAspectRatio(W, 640, 480);
		glfwMakeContextCurrent(W);
	}

};

