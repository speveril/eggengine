#include <string>

#include "core.h"

//----------------------------------------------------------------------------
// Globals

EggCore *core;

//----------------------------------------------------------------------------
// Entry point, call _main from OS-specific entry point.
int _main(std::string cmdLine) {
	core = new EggCore();
	core->start(cmdLine);
	return 0;
}

//----------------------------------------------------------------------------
// Implementation of EggCore

EggCore::EggCore() {
	screen = 0;
}

EggCore::~EggCore() {
	stop();
}

void EggCore::start(std::string cmdLine) {
	Log::write("Engine boot up.");

	scriptEngine = new ScriptEngine();

	std::string cfgFile = cmdLine;
	if (cfgFile.empty())
		cfgFile = "default.cfg";
	else
		cfgFile += ".cfg";
	loadConfiguration(cfgFile);

	if (!config.noWindow) {
		screen = new EggWindow(config.xRes, config.yRes);
		screen->setTitle(config.windowTitle);
		renderEngine = new RenderEngine(screen);
	}

	Log::debug("Egg startup finished. Executing main()");

	scriptEngine->callFunction("main");

	while(1) {
		tick();
	}
}

void EggCore::tick() {
	static double accum = 0;
	static int frames = 0;

	double t = getTime();
	double elapsed = t - lastFrameTime;
	lastFrameTime = t;

	frames++;
	accum += elapsed;
	if (accum >= 1.0) {
		Log::debug("%d fps", frames);
		frames = 0;
		accum = 0.0;
	}

	eventUpdate(elapsed);
	render();

	screen->process();
}

void EggCore::eventKeyDown(unsigned int key) {
	scriptEngine->callFunction("keydown", 1, (double)key);
}

void EggCore::eventKeyUp(unsigned int key) {
	scriptEngine->callFunction("keyup", 1, (double)key);
}

void EggCore::eventUpdate(double dt) {
	scriptEngine->callFunction("update", 1, dt);
}

void EggCore::render() {
	renderEngine->render();
}

void EggCore::stop() {
	Log::write("Engine shut down.");

	if (screen) {
		EggWindow *w = screen;
		screen = 0;
		delete w;
	}

	sysExit();
}

