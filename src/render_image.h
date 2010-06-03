#ifndef _EGG_RENDER_IMAGE_H
#define _EGG_RENDER_IMAGE_H

class RenderImage { 
private:
	void *internals;
	char *_filename;

public:
	typedef enum pixelformat { UNKNOWN, RGBA, RGB, BGRA, BGR, I8 } PixelFormat;

	RenderImage(char *filename);
	~RenderImage();

	int getWidth();
	int getHeight();
	void *getPixels();
	char *getFilename();
	PixelFormat getPixelFormat();
};

#endif