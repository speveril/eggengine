#ifndef _EGG_SCRIPT_H
#define _EGG_SCRIPT_H

#include "core.h"
#include <vector>

class ScriptEngine {
private:
	void *internals;

public:
	enum type { VoidType, NumberType, PointerType };

	ScriptEngine();
	~ScriptEngine();

	bool registerFunction(void *func, const char *name, ScriptEngine::type returnType, unsigned int argc, ...);
	bool loadFile(const char *filename);
	bool loadChunk(void *chunk);
	
	void callFunction(const char *name);

	void registerEggLibraryFunctions();

	void debug();
};

#endif