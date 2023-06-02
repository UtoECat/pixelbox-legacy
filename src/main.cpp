#include "world.h"

using namespace pixelbox;

World w;

#include <assert.h>
#include <window.h>

static void (sqllog) (void*, int code, const char* s) {
	if (s != nullptr) fprintf(stderr, "SQL LOG : %s\n", s);
	else if (code != SQLITE_OK)
	fprintf(stderr, "SQL ERROR CODE : %s\n", sqlite3_errstr(code));
}

static inline chunk_coord C2W_cast(int32_t pos, chunk_coord scale) {
	if (pos < 0) pos -= scale;
	return (chunk_coord)(pos / (int32_t)scale);
}

static inline atom_coord inchunkpos(int32_t pos, chunk_coord scale) {
	int32_t div = pos % (int32_t)scale;
	if (div < 0) div = scale + div;
	return (atom_coord)div;
}	

int mmain() {
	sqlite3_config(SQLITE_CONFIG_LOG, sqllog, nullptr); 
	Storage storage(":memory:");//"./world.db");
	w.setStorage(&storage);
	chunk_position pos = {10, 10};
	Chunk* c = w.getChunk(pos);
	assert(c != nullptr);	
	assert(c->position == pos);	

	// RAII window
	Window win;
	int32_t camx = 0;
	int32_t camy = 0;
	int32_t oldmx = 0;
	int32_t oldmy = 0;
	int k = 1;
	int kind = 45;
	w.setCamera(camx, camy, 100, 100);

	while (!win.shouldExit()) {
		// camera
		int32_t width = win.getWidth() / 4;
		int32_t height = win.getHeight() / 4;
		w.setCamera(camx, camy, width, height);

		// render
		win.begin();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_COLOR_MATERIAL);
		glColor3f(1,	1, 1);
		w.render();
		glUseProgram(0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_2D, 0);
		glColor4f(1, 0, 0, 1);
		glBegin(GL_TRIANGLES);
		glVertex3f(0, 0, 0);
		glVertex3f(-0.5, 0, 0);
		glVertex3f(-0.5, -0.5, 0);
		glEnd();
		win.end();

		// for some reason it is better :/
		w.collectGarbage();
		
		// process
		w.process();
		w.collectGarbage();

		// input
		int button = win.getButton(0);
		int32_t mx = (win.getMouseX()/4/(float)width) * w.camw;
		int32_t my = (win.getMouseY()/4/(float)height) * w.camh;
		if (button) {
			int32_t wx = w.camx + mx;
			int32_t wy = w.camy + my;
			chunk_position pos = {C2W_cast(wx, CHUNK_WIDTH),
				C2W_cast(wy, CHUNK_HEIGHT)};
			atom_coord ax = inchunkpos(wx, CHUNK_WIDTH),
								 ay = inchunkpos(wy, CHUNK_HEIGHT);
			Chunk *c = w.getChunk(pos);
			(*c)[ax][ay] = (Atom){kind | ((c->getRandom() & 3) << 6), c->getRandom()};
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

	w.unloadAll();
	w.releaseRender();
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
