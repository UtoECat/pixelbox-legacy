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
#include <draw/gl.h>
#include <GLFW/glfw3.h>

extern uint8_t colormap_arr[256 * 256 * 3];

// drawing

void draw_clear(void);
void draw_camera(float x, float y, float w, float h);
void draw_rect(float x, float y, float w, float h, float z);
void draw_color(float r, float g, float b);

// NEW API
/****************************************************************
 *
 * Textures
 *
 ****************************************************************
 */
typedef struct gl_texture {
	GLuint type, format, gpuformat;
	GLuint id;
} d_texture_t;

/*
 * Initializes texture structure at pointer t
 * @arg pointer on texture structure
 * @arg type of texture : GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP and etc.
 * @ret 0 on sucess
 *
 * Default texture type is GL_RGB
 * THis function will rebind texture on active slot to new one
 */
int  d_texture_init(d_texture_t* t, GLuint type);

/*
 * Texture init with extra parameters
 * @arg pointer on texture structure
 * @arg type of texture.
 * @arg format of source texture. (GL_RGB, GL_RGBA...)
 * @arg format of texture on GPU (GL_RGB, GL_RGBA, GL_RGB565 and etc.)
 *
 * Set source and gpu format same if possible, to increase perfomance.
 * THis function will rebind texture on active slot to new one
 */
int  d_texture_init_ex(d_texture_t* t, GLuint type, GLuint format, GLuint gpufmt);

/*
 * Loads texture into GPU.
 * Warning: This operation in slower than texture_subset, because of
 * reallocations on GPU to fit textures with new size!
 * You should to use this as little as possible in redrawing loop.
 *
 * @arg pointer on texture structure
 * @arg source data to load from (type must be unsigned char!)
 * @arg width of source data
 * @arg height of source data
 * @ret 0 on sucess.
 * 
 * Texture must be initialized before calling this function.
 * By default, type of data is unsigned char (can't be chaged), but
 * channels count (RGB or RGBA or more) CAN be changed using texture_init_ex
 * 
 * THis function will rebind texture on active slot to this
 */
int  d_texture_load(d_texture_t* t, const uint8_t* data, int w, int h);

/*
 * Uninitializes texture structure at pointer t
 * Don't leave your textures uninitialized
 */
void d_texture_free(d_texture_t* t);

/*
 * Updates texture data from array.
 * Texture data will be copied from application memory to GPU memory, so
 * you can use/free/realloc your data source array without a headache.
 *
 * @arg data source
 * @arg x position on texture in GPU (max = gpu texture width-1)
 * @arg y position on texture in GPU (max = gpu texture height-1)
 * @arg width of region to copy in texture on GPU (max = GPU texture width - x - 1) (width of source data)
 * @arg height of region to copy in texture on GPU (height of source data)
 * @ret 0 on sucess
 *
 * function will fail in case source texture is larger than texture on GPU,
 * or texture has not any data and size (YOU MUST CALL texture_load FIRST!)
 *
 * if you need to resize texture, use texture_load on existing texture, but
 * don't do this so often. (read more in texture_load description).
 *
 * By default, type of data is unsigned char (can't be chaged), but
 * channels count (RGB or RGBA or more) CAN be changed using texture_init_ex
 *
 * THis function will rebind texture on active slot to this
 */
int  d_texture_subset(d_texture_t* t, const uint8_t* data, int x, int y, int w, int h);

/*
 * Binds texture to slot, and sets this slot as active.
 * Slots are (GL_TEXTURE0 ... GL_TEXTURE31) + slot
 *
 * Slots used to make it possible to pass two or more textures in shaders.
 * You can change current active texture using
 * glActiveTexture(GL_TEXTURE0 + slot);
 */
void d_texture_bind(d_texture_t* t, GLuint slot);

/****************************************************************
 *
 * Shaders
 *
 ****************************************************************
 */

typedef struct gl_shaderprog {
	GLuint program;
} d_shader_t;

/*
 * Initializes shader program structure
 */
int  d_shader_init(d_shader_t* s);

/*
 * Uninitializes shader program structure.
 * All shaders attached to it will be uninitialized by opengl automaticly...
 * or should?
 */ 
void d_shader_free(d_shader_t* s);

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
int d_shader_add(d_shader_t* s, const char* source, GLuint type);

/*
 * Links all attached shaders in shader program.
 *
 * @arg shader program structure
 * @ret 0 on sucess.
 *
 * link errors will be printed on stderr :)
 */
int d_shader_link(d_shader_t* s);

/*
 * Binds shader. 
 * After bind, all drawning stuff will go trough this shader program!
 * If you want to unbind int, and return to default legacy drawing, call this with NULL as secnd argument, or call glUseProgram(0);
 */
int d_shader_bind(d_shader_t* s);

/*
 * Founds uniform variable id in shader.
 * You can set value in this variable using glUniform*() functions :)
 * @ret -1 in case of error.
 *
 * WARNING: THis function requires program to be binded! Or it will bind it itself, and not deactivate back!
 */
GLint d_shader_uniform_id(d_shader_t* s, const char* var);

/*
 * Pass legacy opengl GL_PROJECTION_MATRIX and GL_MODELVIEW_MATRIX once.
 * To uniform variables "mProjection" and "mModelview".
 * WARNING: THis function requires program to be binded! Or it will bind it itself, and not deactivate back!
 */
void d_shader_pass_matrixes(d_shader_t* s);
