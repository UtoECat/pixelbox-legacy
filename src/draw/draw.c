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
#include <draw/draw.h>

int draw_init(void) {
	return 0;
}

void draw_free(void) {

}

void draw_clear(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void draw_camera(float x, float y, float w, float h) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(x, x+w, y+h, y, -100, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void draw_rect(float x, float y, float w, float h, float z) {
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(x, y, z);
	glVertex3f(x+w, y, z);
	glVertex3f(x+w, y+h, z);
	glVertex3f(x, y+h, z);
	glEnd();
}

void draw_color(float r, float g, float b) {
	glColor3f(r, g, b);
}

// NEW API
/****************************************************************
 *
 * Textures
 *
 ****************************************************************
 */

#include <gl.h>

// from window.c
const char* gl_error_string(GLenum err); 

static int local_check_gl_error(d_texture_t* t, const char* msg) {
	int err = glGetError();
	if (err == GL_NO_ERROR) return 0;
	errorf("Texture (%i) : %s at stage %s", t->id, gl_error_string(err), msg);
	return 1;
}
static int locsh_check_gl_error(d_shader_t* s, const char* msg) {
	int err = glGetError();
	if (err == GL_NO_ERROR) return 0;
	errorf("Shader program (%i) : %s at stage %s", s->program, gl_error_string(err), msg);
	return 1;
}

int  d_texture_init(d_texture_t* t, GLuint type) {
		return d_texture_init_ex(t, type, GL_RGB, GL_RGB);
}

int  d_texture_init_ex(d_texture_t* t, GLuint type, GLuint format, GLuint gpufmt) {
	if (!t) return 2;
	glGenTextures(1, &t->id);
	if (local_check_gl_error(t, "texture object initialization")) return 1;
	if (!t->id) return 1;
	glBindTexture(type, t->id);
	if (local_check_gl_error(t, "texture object binding to type")) return 1;
	t->type = type;
	t->format = format;
	t->gpuformat = gpufmt;
	return 0;
}

int  d_texture_load(d_texture_t* t, const uint8_t* data, int w, int h) {
	if (!t) return 2;
	glBindTexture(t->type, t->id);
	if (local_check_gl_error(t, "texture object binding to type")) return 1;
	glTexParameteri(t->type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(t->type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	if (local_check_gl_error(t, "texture object filtering setup")) return 1;
	glTexParameteri(t->type, GL_TEXTURE_WRAP_S, GL_REPEAT); 	
	glTexParameteri(t->type, GL_TEXTURE_WRAP_T, GL_REPEAT);
	if (local_check_gl_error(t, "texture object properties setup")) return 1;
	glTexImage2D(t->type, 0, t->format, w, h, 0, t->gpuformat, GL_UNSIGNED_BYTE, data);	
	if (local_check_gl_error(t, "texture object loading (glTexImage2D)")) return 1;
	return 0;
}

/*
 * Uninitializes texture structure at pointer t
 * Don't leave your textures uninitialized
 */
void d_texture_free(d_texture_t* t) {
	if (!t) return;
	glDeleteTextures(1, &t->id);
	if (local_check_gl_error(t, "texture object freeing")) return;
}

int  d_texture_subset(d_texture_t* t, const uint8_t* data, int x, int y, int w, int h) {
	if (!t) return 2;	
	if (!t->id) return 3;
	glBindTexture(t->type, t->id);
	if (local_check_gl_error(t, "texture object binding to type")) return 1;
	// update texture data (texture size MUST BE SAME!)
	glTexSubImage2D(t->type, 0, x,y,w,h, t->format, GL_UNSIGNED_BYTE, data);
	if (local_check_gl_error(t, "texture object loading(glTexSubImage2D)")) return 1;
	return 0;
}

void d_texture_bind(d_texture_t* t, GLuint slot) {
	slot += GL_TEXTURE0;
	if (slot > GL_TEXTURE31) {
		errorf("Texture (%i) : binding to invalid GL_TEXTURE%i", t->id, slot - GL_TEXTURE0);
		return;
	};
	glActiveTexture(slot);
	if (!t || !t->id) {
		glBindTexture(GL_TEXTURE_2D, 0);
		return;
	}
	glBindTexture(t->type, t->id);
	if (local_check_gl_error(t, "texture object binding")) return;
}

/****************************************************************
 *
 * Shaders
 *
 ****************************************************************
 */

/*
 * Initializes shader program structure
 */
int  d_shader_init(d_shader_t* s) {
	if (!s) return 2;
	s->program = glCreateProgram();
	if (locsh_check_gl_error(s, "program creation")) return 1;
	if (!s->program) return 1;
	return 0;
}

/*
 * Uninitializes shader program structure.
 * All shaders attached to it will be uninitialized by opengl automaticly...
 * or should?
 */ 
void d_shader_free(d_shader_t* s) {
	glDeleteProgram(s->program);
}

/*
 * Reads, compiles shader source, and attaches result to shader program.
 *
 * @arg shader program structure
 * @arg shader source string
 * @arg shader type (GL_FRAGMENT_SHADER, GL_VERTEX_SHADER, and etc.)
 * @ret 0 on sucess.
 *
 * compile error will be printed on stderr :)
 * BUG: some nvidia opengl implementations may not like destroying gl shader before program linking
 */
int d_shader_add(d_shader_t* s, const char* source, GLuint type) {
	if (!s) return 2;
	if (type != GL_VERTEX_SHADER && type != GL_FRAGMENT_SHADER) {
		errorf("Shader program (%i) : bad shader type %i!", s->program, type);
		return 2;
	}
	if (!source) return 2;
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	char tmp[1024] = {0};
	int success = 1;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 1024, NULL, tmp);
		errorf("Shader (%i) compiling error : %s ", shader, tmp);
		glDeleteShader(shader);
		return 2;
	}
	glAttachShader(s->program, shader);
	glDeleteShader(shader);
	if (locsh_check_gl_error(s, "temporary shader compiling or attachment")) return 1;
	return 0;
}

/*
 * Links all attached shaders in shader program.
 *
 * @arg shader program structure
 * @ret 0 on sucess.
 *
 * link errors will be printed on stderr :)
 */
int d_shader_link(d_shader_t* s) {
	if (!s) return 2;
	glLinkProgram(s->program);
	char tmp[1024] = {0};
	int success = 1;
	glGetProgramiv(s->program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(s->program, 1024, NULL, tmp);
		errorf("Shader program (%i) linking error : %s", s->program, tmp);
		return 2;
	}
	if (locsh_check_gl_error(s, "program linking")) return 1;
	return 0;
}

/*
 * Binds shader. 
 * After bind, all drawning stuff will go trough this shader program!
 * If you want to unbind int, and return to default legacy drawing, call this with NULL as secnd argument, or call glUseProgram(0);
 */
int d_shader_bind(d_shader_t* s) {
	if (!s) {
		glUseProgram(0);		 
		return 0;
	};
	glUseProgram(s->program);
	if (locsh_check_gl_error(s, "program binding")) return 1;
	return 0;
}

GLint d_shader_uniform_id(d_shader_t* s, const char* var) {
	if (!s || !var) return -1;
	GLint id; glGetIntegerv(GL_CURRENT_PROGRAM,&id);
	if ((GLuint)id != s->program) glUseProgram(s->program);
	return glGetUniformLocation(s->program, var);
}

void d_shader_pass_matrixes(d_shader_t* s) {
	if (!s) return;
	GLint id; glGetIntegerv(GL_CURRENT_PROGRAM,&id);
	if ((GLuint)id != s->program) glUseProgram(s->program);

	GLfloat model[16], proj[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, model); 
	glGetFloatv(GL_PROJECTION_MATRIX, proj);
	int p = d_shader_uniform_id(s, "mProjection");
	int m = d_shader_uniform_id(s, "mModelview");
	if (p == -1 || m == -1) {
		errorf("Can't get %s matrix!", (p == -1) ? "mProjection" : "mModelview");
		return;				
	}
	glUniformMatrix4fv(p, 1, GL_FALSE, proj);
	glUniformMatrix4fv(m, 1, GL_FALSE, model);
	return;
}
