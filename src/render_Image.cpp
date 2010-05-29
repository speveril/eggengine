#include "core.h"
#include "render_image.h"
#include "corona/corona.h"

// TODO write a caching mechanism so RenderImages only get loaded once

typedef struct CoronaRenderImageInternals {
	corona::Image *img;
} Internals;

RenderImage::RenderImage(char *filename) {
	internals = malloc(sizeof(Internals));

	Internals *i = (Internals *)internals;

	corona::File *f = corona::OpenFile("test3.png", false);
	if (!f) Log::error("Could not open image file %s", filename);
	i->img = corona::OpenImage(f);
}

RenderImage::~RenderImage() {
	Internals *i = (Internals *)internals;
	delete i->img;
}

int RenderImage::getWidth() {
	Internals *i = (Internals *)internals;
	return i->img->getWidth();
}

int RenderImage::getHeight() {
	Internals *i = (Internals *)internals;
	return i->img->getWidth();
}

void *RenderImage::getPixels() {
	Internals *i = (Internals *)internals;
	return i->img->getPixels();
}

RenderImage::PixelFormat RenderImage::getPixelFormat() {
	Internals *i = (Internals *)internals;
	switch (i->img->getFormat()) {
		case corona::PF_R8G8B8A8: return RGBA;
		case corona::PF_R8G8B8: return RGB;
		case corona::PF_B8G8R8A8: return BGRA;
		case corona::PF_B8G8R8: return BGR;
		case corona::PF_I8: return I8;
		default:
			Log::error("Unknown pixel format in image.");
			return UNKNOWN;
	}

}
