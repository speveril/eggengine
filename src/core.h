#ifndef _EGG_CORE_H
#define _EGG_CORE_H

#include "config.h"
#include <string>

//----------------------------------------------------------------------------
// OS specific includes
#ifdef OS_WINDOWS
#define NOMINMAX
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#elif OS_MAC
// mac headers
#elif OS_UNIX
// *nix headers
#endif

class EggWindow;

//----------------------------------------------------------------------------
// Global includes
#include "gen_util.h"

//----------------------------------------------------------------------------
// Core Functions
int _main(std::string cmdLine);          // implemented in core.cpp
void sysExit(int code = 0);              // implemented in os_*.cpp

//----------------------------------------------------------------------------
// Core Classes

// ...
class Log { // implemented in core_Log.cpp
public:
	static void write(const char *msg, ...);
	static void error(const char *msg, ...);

#ifdef DEBUG
	static void debug(const char *msg, ...);
#else
	static void debug(const char *msg, ...) {} // if we're not in debug mode, inline debug() so it just goes away
#endif

};

// ...
class EggWindow { // implemented in os_*.cpp
private:
	void *internals;
	unsigned int w, h;

public:
	EggWindow(unsigned int w, unsigned int h);
	~EggWindow();

	void process();
	double getTime();
	unsigned int getHeight();
	unsigned int getWidth();
	void setHeight(unsigned int h);
	void setWidth(unsigned int w);

	void prepareRender();
	void teardownRender();

	void setTitle(std::string title);
};

// Component includes
#include "script.h"
#include "render.h"

// ...
class EggCore { // implemented in core.cpp
private:
	EggWindow *screen;
	ScriptEngine *scriptEngine;
	RenderEngine *renderEngine;
	double lastFrameTime;
	struct {
		int xRes, yRes;
		bool noWindow;
		std::string windowTitle;
	} config;

public:
	EggCore();
	~EggCore();

	void start(std::string cmdLine);
	void tick();
	void render();
	void stop();

	void loadConfiguration(std::string filename);

	double getTime() { return screen->getTime(); }
	EggWindow *getScreen() { return screen; }
};

//----------------------------------------------------------------------------
// Core globals
extern EggCore *core;

#endif