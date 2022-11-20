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

#include <main.h>
#include <galogen/gl.h>
#include <GLFW/glfw3.h>
#include <stddef.h>
#include <string.h>

static GLFWwindow* win = NULL;

static void (glfwerrcb) (int i, const char* c) {
	errorf(0, "GLFW Error %s (code %i)!", c, i);
}

static void (resizecb) (GLFWwindow*, int w, int h) {
	glViewport(0, 0, w, h);
}


static const char* title = "PixelBox 1.0 : ";
static const size_t title_size = 15;

// export music system functions
int music_init (void);
void music_free (void);
void music_tick(void);
void init_pixel_types(void);
void free_pixel_types(void);

// shader init/free
void init_shaders();
void free_shaders();

void main_free() {
	music_free();
	free_shaders();
	glfwDestroyWindow(win);	
	glfwTerminate();
	free_pixel_types();
}

void (set_status) (const char* c) {
	char buff[strlen(c) + title_size + 2];
	memcpy(buff, title, title_size);
	memcpy(buff + title_size, c, strlen(c) + 1); 
	glfwSetWindowTitle(win, buff);
}

int  (should_exit) (void) {
	return glfwWindowShouldClose(win);
}

int main_load() {
	init_pixel_types();
	debugf("GLFW Version = %s!", glfwGetVersionString());
	glfwSetErrorCallback(glfwerrcb);
	if (glfwInit() != GLFW_TRUE) {
		errorf(0, "Can't init GLFW3!");
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	win = glfwCreateWindow(640, 480, "PixelBox 1.0", NULL, NULL);
	if (!win) {
		errorf(0, "Can't open window!");
		return -2;
	}
	glfwSetFramebufferSizeCallback(win, resizecb);
	glfwSetWindowSizeLimits(win, 640, 480, GLFW_DONT_CARE, GLFW_DONT_CARE);
	glfwSetWindowAspectRatio(win, 640, 480);
	glfwMakeContextCurrent(win);
	init_shaders();

	if (music_init() != 0) {
		errorf(0, "Audio is disabled :)");
	}
	return 0;
}

const char* gl_error_string(GLenum err) {
  switch (err) {
    case GL_INVALID_ENUM:
      return "GL_INVALID_ENUM";

    case GL_INVALID_VALUE:
      return "GL_INVALID_VALUE";

    case GL_INVALID_OPERATION:
      return "GL_INVALID_OPERATION";

    case GL_STACK_OVERFLOW:
      return "GL_STACK_OVERFLOW";

    case GL_STACK_UNDERFLOW:
      return "GL_STACK_UNDERFLOW";

    case GL_OUT_OF_MEMORY:
      return "GL_OUT_OF_MEMORY";

		default:
			return "UNKNOWN_ERROR";
	}
}

#include <stdlib.h>
void gl_check_error(const char* stage) {
	#ifndef NDEBUG
	GLenum err;
	if ((err = glGetError()) != GL_NO_ERROR) {
		errorf(0, "OpenGL Error %s (%i)", gl_error_string(err), err);
		errorf(0, "OpenGL Error happened at stage %s", stage);
		errorf(0, "Aborting...");
		abort();
	}
	#endif
}

void main_tick (void) {
	music_tick();
	glfwPollEvents();
	glfwSwapBuffers(win);
	glfwMakeContextCurrent(win);
	GLenum err; int bad = 0;
	while((err = glGetError()) != GL_NO_ERROR) {
  	errorf(0, "OpenGL Error %s (%i)", gl_error_string(err), err);
		bad = 1;
	}
	if (bad) {
		glfwSetWindowShouldClose(win, 1);
		errorf(0, "OpenGL Errors founded. No sense to continue...");
	}
}

int   (get_key)    (int i) {
	return glfwGetKey(win, i);
}

int   (get_button) (int i) {
	return glfwGetMouseButton(win, i);
}

float (mouse_x) (void) {
	double v = 0.0;
	glfwGetCursorPos(win, &v, NULL);
	return v;
}

float (mouse_y) (void) {
	double v = 0.0;
	glfwGetCursorPos(win, NULL, &v);
	return v;
}
