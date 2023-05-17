#include "world.h"

using namespace pixelbox;

World w;

#include <assert.h>

int main() {
	w.setDirectory("./world1/");
	chunk_position pos = {10, 10};
	Chunk* c = w.getChunk(pos);
	assert(c != nullptr);	
	assert(c->position == pos);	
	w.collectGarbage();
	w.collectGarbage();
	w.collectGarbage();
	w.collectGarbage();
	w.collectGarbage();
	w.collectGarbage();
	return 0;
}
