#include "world.h"

using namespace pixelbox;

World w;

#include <assert.h>
#include <window.h>
#include <math.h>

static inline chunk_coord C2W_cast(int32_t pos, chunk_coord scale) {
	if (pos < 0) pos -= scale;
	return (chunk_coord)(pos / (int32_t)scale);
}

static inline atom_coord inchunkpos(int32_t pos, chunk_coord scale) {
	int32_t div = pos % (int32_t)scale;
	if (pos < 0) div += scale-1;
	return (atom_coord)div;
}	

// RAII window
Window win;

void WorldRenderFunction() {
	// render
	glColor3f(1,	1, 1);
	w.render();
	glUseProgram(0);
	glClear(GL_DEPTH_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, 0);
	glColor4f(1, 1, 1, 1);
	// to keep memory available
	w.collectGarbage();	
}

static int32_t oldmx = 0;
static int32_t oldmy = 0;
static int k = 1;
static int kind = 45;
static int32_t camx = 0;
static int32_t camy = 0;

void WorldProcessFunction() {
	// camera
	int camwidth = win.getWidth() / 4;
	int camheight = win.getHeight() / 4;
	w.setCamera(camx, camy, camwidth, camheight);
	
	// process
	w.process();
	w.collectGarbage();
}

void drawRectangle(int x, int y, int w, int h) {
	glBegin(GL_POLYGON);
	glVertex2f(x, y);
	glVertex2f(x+w,y);
	glVertex2f(x+w, y+h);
	glVertex2f(x, y+h);
	glEnd();
}

Storage * storptr;

void GUIFunction() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w.camw, w.camh, 0, 100, -100);
	glMatrixMode(GL_MODELVIEW);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	glLoadIdentity();

	if (storptr->working())
		glColor4f(1, 1, 1, 1);
	else glColor4f(1, 1, 0, 1);
	drawRectangle(20, 10, 20, 20);
}

// original taken from : https://gist.github.com/bert/1085538
// highly modified for my purposes. WARNING: works for one chunk ONLY
void chunkPlotLine (int x0, int y0, int x1, int y1) {
  int dx =  abs (x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs (y1 - y0), sy = y0 < y1 ? 1 : -1; 
  int err = dx + dy, e2; /* error value e_xy */
 
	chunk_position oldpos = {C2W_cast(x0, CHUNK_WIDTH), C2W_cast(y0, CHUNK_HEIGHT)};
	Chunk* c = w.getChunk(oldpos);
  for (;;){  /* loop */
		chunk_position pos = {C2W_cast(x0, CHUNK_WIDTH), C2W_cast(y0, CHUNK_HEIGHT)};
		if (pos != oldpos) c = w.getChunk(pos);
		atom_coord ax = inchunkpos(x0, CHUNK_WIDTH),
							 ay = inchunkpos(y0, CHUNK_HEIGHT);
		int16_t rngnum = c->getRandom();
		Atom atom = {kind | ((rngnum & 3) << 6), rngnum ^ (rngnum * 345678)};
    (*c)[ax][ay] = atom;

    if (x0 == x1 && y0 == y1) break;
    e2 = 2 * err;
    if (e2 >= dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
    if (e2 <= dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
  }
}

void UserInputFunction() {
	int32_t mx = win.getMouseX()/4;
	int32_t my = win.getMouseY()/4;
	if (win.getButton(0)) {
		int32_t wx = w.camx + mx;
		int32_t owx = w.camx + oldmx;
		int32_t wy = w.camy + my;
		int32_t owy = w.camy + oldmy;
		chunkPlotLine(owx, owy, wx, wy);
	}
	if (win.getButton(1)) {
		int32_t dx = oldmx - mx;
		int32_t dy = oldmy - my;
		camx += dx;
		camy += dy;
	}
	oldmx = mx;
	oldmy = my;
	if (win.getKey(GLFW_KEY_MINUS) && kind > 0) kind -= 1;
	if (win.getKey(GLFW_KEY_EQUAL) && kind+1 < TYPES_COUNT) kind += 1;
	if (win.getKey(GLFW_KEY_W)) camy -= k;
	if (win.getKey(GLFW_KEY_S)) camy += k;
	if (win.getKey(GLFW_KEY_A)) camx -= k;
	if (win.getKey(GLFW_KEY_D)) camx += k;
}

int mmain() {
	Storage storage("./world.db");
	storptr = &storage;
	storage.startAsyncThread();
	w.setStorage(&storage);
	w.setCamera(camx, camy, 100, 100);

	WorldProcessFunction();
	while (!win.shouldExit()) {
		WorldProcessFunction();
		win.begin();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_COLOR_MATERIAL);
		WorldRenderFunction();
		GUIFunction();
		win.end();
		UserInputFunction();
	}
	w.unloadAll();
	w.releaseRender();
	storage.stopAsyncThread();
	return 0;
}

int main() {
	try {return mmain();}
	catch (const char* e) {
		fprintf(stderr, "Exeption %s!\n", e);
	} catch (...) {
		fprintf(stderr, "Exeption unknown!\n");
	}
	fflush(stdout);
	fprintf(stderr, "Attempt to terminate normally...\n");
	return -1;
}
