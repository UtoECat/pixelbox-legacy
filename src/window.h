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

#include "gl.h"
#include <GLFW/glfw3.h>
#include <cstdio>

namespace pixelbox {

	class Window {
		private:
		class GLFW3class {
			friend class Window;
			private:
			size_t refcnt = 0;
			static void errcb	(int, const char* c) {
				fprintf(stderr, "GLFW Error %s!", c);
			}
			public:
			GLFW3class() {
				glfwSetErrorCallback(errcb);
				if (glfwInit() != GLFW_TRUE) throw "can't load GLFW3!";
				fprintf(stderr, "GLFW3 : OK (%p)\n", this);
			}
			~GLFW3class() {
				glfwTerminate();
			}
			void refinc() {refcnt++;};
			void refdec() {refcnt--; if (!refcnt) delete this;}
		};
		protected:
		static GLFW3class *GLFW3;
		static void (resizecb) (GLFWwindow*, int w, int h) {
			glViewport(0, 0, w, h);
		}
		GLFWwindow *W = nullptr;
		public:
		Window();
		~Window() {
			glfwDestroyWindow(W);	
			GLFW3->refdec();
		}
		void begin() {
			glfwSwapBuffers(W);
			glfwMakeContextCurrent(W);
		}
		void end() {
			glFlush();
			glfwPollEvents();
		}
		bool shouldExit (void) {
			return glfwWindowShouldClose(W);
		}

		int   (getKey)    (int i) {
			return glfwGetKey(W, i);
		}

		int   (getButton) (int i) {
			return glfwGetMouseButton(W, i);
		}

		float (getMouseX) (void) {
			double v = 0.0;
			glfwGetCursorPos(W, &v, NULL);
			return v;
		}

		float (getMouseY) (void) {
			double v = 0.0;
			glfwGetCursorPos(W, NULL, &v);
			return v;
		}

		int getWidth() {
			int v = 0;
			glfwGetWindowSize(W, &v, NULL);
			return v;
		}

		int getHeight() {
			int v = 0;
			glfwGetWindowSize(W, NULL, &v);
			return v;
		}
	};

};
