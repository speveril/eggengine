#include "core.h"
#include "render.h"

#ifdef DEBUG
void checkGLErrors(const char *msg) {
	int e = glGetError();
	if (e != GL_NO_ERROR) {
		Log::error("OpenGL Error (%0x) %s, %s", e, gluErrorString(e), msg);
	}
}
#else
inline void checkGLErrors(const char *msg) {}
#endif

RenderEngine::RenderEngine(EggWindow *s) {
	Log::debug("Constructing OpenGL RenderEngine.");
	
	if (s) win = s;
	else win = core->getScreen();

	win->prepareRender();

	if (!win) {
		Log::error("Could not find a window for this rendering context.");
		core->stop();
	}

	glClearColor(.5, .5, .5, 1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	currentStack = new RenderStack();

	debug();

	checkGLErrors("RenderEngine constructor");

	win->teardownRender();
}

RenderEngine::~RenderEngine() {
	Log::debug("Destroying OpenGL RenderEngine.");
}

void RenderEngine::debug() {
	Log::debug("Do RenderEngine debug calls.");

	//std::vector<Layer *> *stack = new std::vector<Layer *>();
	RenderStack *stack = getRenderStack();

	RenderImage *img = new RenderImage("test3.png");

	double h = (double)win->getHeight();
	double w = (double)win->getWidth();
	double left, right, bottom, top;
	if (w > h) {
		top = -1.0; bottom = 1.0;
		left = -(double)w/h; right = (double)w/h;
	} else {
		top = -(double)h/w; bottom = (double)h/w;
		left = -1.0; right = 1.0;
	}

	Layer *lyr = new Layer(top, bottom, left, right);

	for (double x = -1.0; x <= 1.0; x += 0.1) {
		lyr->elements.push_back(new Point(x, -1.0));
		lyr->elements.push_back(new Point(x, 0.0));
		lyr->elements.push_back(new Point(x, 1.0));
	}
	for (double y = -1.0; y <= 1.0; y += 0.1) {
		lyr->elements.push_back(new Point(-1.0, y));
		lyr->elements.push_back(new Point(0.0, y));
		lyr->elements.push_back(new Point(1.0, y));
	}

	stack->push_back(lyr);

	lyr = new Layer(0, 240, 0, 320);
	Sprite *s = new Sprite(img, 100, 75, 64, 64);
	//s->setFrameDimensions(0.5, 0.5);
	//s->setFrame(1,1);
	lyr->elements.push_back(s);
	s = new Sprite(img, 260, 150, 128, 128);
	//s->setFrameDimensions(0.5, 0.5);
	//s->setFrame(0,0);
	s->rotateTo(30);
	lyr->elements.push_back(s);
	stack->push_back(lyr);

	currentStack = stack;

	delete img;

	Log::debug("Done RenderEngine debug calls.");
}

void RenderEngine::render() {
	win->prepareRender();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT);
	checkGLErrors("frame setup");

	// process current renderstack
	if (currentStack && currentStack->size() > 0) {
		std::vector<Layer *>::iterator i;
		for (i = currentStack->begin(); i != currentStack->end(); i++) {
			(*i)->render();
		}
	}
	checkGLErrors("end of render");
	win->teardownRender();
}

void Layer::render() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	//gluPerspective(90.0, (double)win->getHeight() / (double)win->getHeight(), 0.001, 100.0);

	// For 2d layers
	gluOrtho2D(_left, _right, _bottom, _top);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (elements.size() > 0) {
		glClear(GL_DEPTH_BUFFER_BIT);

		std::vector<RenderObject *>::iterator i;
		for (i = elements.begin(); i != elements.end(); i++) {
			(*i)->fullrender();
		}
	}
}

void Point::fullrender() {
	glBegin(GL_POINTS);
	render();
	glEnd();
}

void Point::render() {
	glVertex2d(_x, _y);
}

// Primitive rendering is pretty inefficient; use batch rendering instead 
// if you can (ONCE I MAKE IT HURRRRRRRR)

void RenderObject2d::position() {
	glTranslated(_x, _y, 0);
	glRotated(_rot, 0, 0, 1);
}

void Rect::fullrender() {
	glPushMatrix();
	position();

	glBegin(GL_QUADS);
	render();
	glEnd();

	glPopMatrix();
}

void Rect::render() {
	double hw = _w/2, hh = _h/2;
	glVertex2d(-hw, -hh);
	glVertex2d(-hw, hh);
	glVertex2d(hw, hh);
	glVertex2d(hw, -hh);
}

Sprite::Sprite(RenderImage *img, double x, double y, double w, double h):Rect(x,y,w,h), _img(img) {
	Log::debug("Creating sprite with image '%s' (%dx%d)", _img->getFilename(), _img->getWidth(), _img->getHeight());

	// Currently this generates a new texture every single time we make a sprite. What it SHOULD do is check
	// a texture cache of some sort so we're not producing duplicates. I'm thinking a map of filename -> texture
	// number.

	core->getScreen()->prepareRender();
	glEnable(GL_TEXTURE_2D);

	_texture = 0;
	glGenTextures(1, &_texture);
	checkGLErrors("creating sprite (generating texture name)");
	glBindTexture(GL_TEXTURE_2D, _texture);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	int numComponents, glformat;

	switch (_img->getPixelFormat()) {
		case RenderImage::RGBA:
			Log::debug("Pixel format in sprite image is RGBA.");
			numComponents = 4; glformat = GL_RGBA;
		break;
		case RenderImage::RGB:
			Log::debug("Pixel format in sprite image is RGB.");
			numComponents = 3; glformat = GL_RGB;
		break;
		case RenderImage::BGRA:
			Log::debug("Pixel format in sprite image is BGRA.");
			numComponents = 4; glformat = GL_BGRA_EXT;
		break;
		case RenderImage::BGR:
			Log::debug("Pixel format in sprite image is BGR.");
			numComponents = 3; glformat = GL_BGR_EXT;
		break;
		case RenderImage::I8:
			Log::debug("Pixel format in sprite image is paletted.");
			Log::error("Paletted images not supported yet.");
		default:
			Log::error("Bad image passed to Sprite.");
			glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_TEXTURE_2D);
			glDeleteTextures(1, &_texture);
			_texture = 0;
			return;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, numComponents, _img->getWidth(), _img->getHeight(), 0, glformat, GL_UNSIGNED_BYTE, _img->getPixels());

	glDisable(GL_TEXTURE_2D);

	Log::debug("Texture number: %u", _texture);

	setFrameDimensions(1, 1);
	setFrame(0, 0);

	core->getScreen()->teardownRender();
}

void Sprite::fullrender() {
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, _texture);

	position();
	glBegin(GL_QUADS);
	render();
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void Sprite::render() {
	double hw = _w/2, hh = _h/2;

	glTexCoord2d(_tx,_ty);
	glVertex2d(-hw, -hh);
	glTexCoord2d(_tx,_ty+_th);
	glVertex2d(-hw, hh);
	glTexCoord2d(_tx+_tw,_ty+_th);
	glVertex2d(hw, hh);
	glTexCoord2d(_tx+_tw,_ty);
	glVertex2d(hw, -hh);
}