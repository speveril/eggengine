#ifndef _EGG_RENDER_H
#define _EGG_RENDER_H

#include <vector>

#include "render_image.h"

class EggWindow;

class RenderObject {
public:
	double _x, _y;
	
	RenderObject(double x, double y):_x(x), _y(y) {}
	~RenderObject() {}

	virtual void fullrender() = 0;
	virtual void render() = 0;
};

class Point : public RenderObject {
public:
	Point(double x, double y):RenderObject(x,y) {}
	~Point() {}

	virtual void render();
	virtual void fullrender();
};

class RenderObject2d : public RenderObject {
public:
	double _rot;
	double _w, _h;
	RenderObject2d(double x, double y, double w, double h):RenderObject(x,y), _w(w), _h(h), _rot(0) {}
	void rotateTo(double r) { _rot = r; }
	void rotateBy(double r) { _rot += r; }
	void position();
};

class Rect : public RenderObject2d {
public:
	Rect(double x, double y, double w, double h):RenderObject2d(x,y,w,h) {}
	virtual void render();
	virtual void fullrender();
};

class Sprite : public Rect {
public:
	unsigned int _texture;
	RenderImage *_img;
	double _tx, _ty, _tw, _th;

	Sprite(RenderImage *img, double x, double y, double w, double h);
	virtual void setFrame(double x, double y) { _tx = x*_tw; _ty = y*_th; }
	virtual void setFrameDimensions(double w, double h) { _tw = w; _th = h; }
	virtual void render();
	virtual void fullrender();
};

class Layer {
private:
	double _left, _right, _top, _bottom;

public:
	Layer(double top, double bottom, double left, double right):_top(top), _bottom(bottom), _left(left), _right(right) {}

	std::vector<RenderObject *> elements;
	void render();
};

typedef std::vector<Layer*> RenderStack;

class RenderEngine {
private:
	EggWindow *win;
	RenderStack *currentStack;

public:
	RenderEngine(EggWindow *s = 0);
	~RenderEngine();

	void debug();
	void render();

	// inline accessor methods
	RenderStack *getRenderStack() { return currentStack; }
	void setRenderStack(RenderStack *stack) { currentStack = stack; }
};

#endif