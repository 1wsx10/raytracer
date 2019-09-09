#include "main.h"


int main() {
	timer main_timer("main timer");

	std::unique_ptr<framebuf, framebuf::deleter> fb = framebuf::make_unique();
	RGBT temp_rgbt = {255, 255, 255, 0};
	pixel_ pix(0, 0, temp_rgbt);

	for(int i = 0; i < 50; i++) {
		static_cast<PIXEL*>(&pix)->x = &i;
		draw(fb.get(), &pix);
	}

	return EXIT_SUCCESS;
}
