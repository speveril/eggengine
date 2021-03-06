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

	bool registerFunction(void *func, const char *name, ScriptEngine::type returnType, unsigned int argc = 0, ...);
	bool loadFile(const char *filename);
	bool loadChunk(void *chunk);
	
	bool hasFunction(const char *name);
	void callFunction(const char *name, unsigned int argc = 0, ...);

	void registerEggLibraryFunctions();

	void debug();
};

#endif