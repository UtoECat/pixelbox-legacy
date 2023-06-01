#include "world.h"

using namespace pixelbox;

World w;

#include <assert.h>
#include <window.h>

static void (sqllog) (void*, int, const char* s) {
	fprintf(stderr, "SQL LOG : %s\n");
}

int mmain() {
	sqlite3_config(SQLITE_CONFIG_LOG, sqllog, nullptr); 
	Storage storage("./world.db");
	w.setStorage(&storage);
	chunk_position pos = {10, 10};
	Chunk* c = w.getChunk(pos);
	assert(c != nullptr);	
	assert(c->position == pos);	

	// RAII window
	Window win;
	int32_t camx = 0;
	int32_t camy = 0;
	int k = 1;
	w.setCamera(camx, camy, 100, 100);

	while (!win.shouldExit()) {
		// camera
		uint32_t width = win.getWidth() / 4;
		uint32_t height = win.getHeight() / 4;
		w.setCamera(camx, camy, width, height);

		// render
		win.begin();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_COLOR_MATERIAL);
		glColor3f(1,	1, 1);
		w.render();
		glUseProgram(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glColor3f(1, 0, 0);
		glBegin(GL_TRIANGLES);
		glVertex3f(0, 0, 10);
		glVertex3f(-0.5, 0, 10);
		glVertex3f(-0.5, -0.5, 10);
		glEnd();
		win.end();
		
		// process
		w.process();
		w.collectGarbage();

		// input
		int button = win.getButton(1);
		int kind = 45;
		if (button) {
			uint32_t wx = w.camx + (win.getMouseX()/4/(float)width) * w.camw;
			uint32_t wy = w.camy + (win.getMouseY()/4/(float)height) * w.camh;
			chunk_position pos = {wx/CHUNK_WIDTH, wy/CHUNK_HEIGHT};
			atom_coord ax = wx%CHUNK_WIDTH, ay = wy%CHUNK_HEIGHT;
			Chunk *c = w.getChunk(pos);
			(*c)[ax][ay] = (Atom){kind | ((c->getRandom() & 3) << 6), c->getRandom()};
		}
		if (win.getKey(GLFW_KEY_MINUS) && k > 1) k -= 1;
		if (win.getKey(GLFW_KEY_EQUAL) && k < 10) k += 1;
		if (win.getKey(GLFW_KEY_W)) camy -= k;
		if (win.getKey(GLFW_KEY_S)) camy += k;
		if (win.getKey(GLFW_KEY_A)) camx -= k;
		if (win.getKey(GLFW_KEY_D)) camx += k;
	}

	w.unloadAll();
	w.releaseRender();
	glfwTerminate();
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
