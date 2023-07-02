#include <raylib.h>
#include <assert.h>

int main(int argc, char** argv) {
	assert(argc == 3);
	Image img = LoadImage(argv[1]);
	ExportImageAsCode(img, argv[2]);
	return 0;
}
