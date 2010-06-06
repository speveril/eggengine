// Library function implementation, LLVM-style.

// See script_LLVM for more ramblings about the LLVM ScriptEngine.

// Turn off warnings generated by LLVM; I don't really care to fix them and I don't
// want them clogging up my compile output.
#pragma warning(disable:4146)
#pragma warning(disable:4996)
#pragma warning(disable:4800)

#include "core.h"
#include "script.h"

#include "llvm/Linker.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Constants.h"
#include "llvm/Instructions.h"
#include "llvm/ModuleProvider.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/Interpreter.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/System/DynamicLibrary.h"
#include "llvm/Bitcode/ReaderWriter.h"

// use the llvm namespace so we're not forever typing "llvm::"
using namespace llvm;

#define SCRIPT_FUNCTION(FUNC_NAME) GenericValue FUNC_NAME(FunctionType *ft, const std::vector<GenericValue> &args)

static GenericValue nullGV;

// - Testing Built In Functions -

SCRIPT_FUNCTION(testCall) {
	Log::debug("testCall() has executed.");
	return nullGV;
}

SCRIPT_FUNCTION(addOne) {
	GenericValue rv;
	rv.DoubleVal = args[0].DoubleVal + 1;

	Log::debug("addOne() executed. Returning %g + 1 = %g", args[0].DoubleVal, rv.DoubleVal);
	
	return rv;
}

SCRIPT_FUNCTION(writeNumberToLog) {
	Log::write("%g", args[0].DoubleVal);
	return nullGV;
}

SCRIPT_FUNCTION(writeStringToLog) {
	Log::write((char *)args[0].PointerVal);
	return nullGV;
}

// - Memory Management Functions -

SCRIPT_FUNCTION(eggfunc_delete) {
	delete args[0].PointerVal;
	return nullGV;
}

// - Graphics Built In Functions -

SCRIPT_FUNCTION(eggfunc_getResolutionX) {
	GenericValue rv;
	rv.DoubleVal = core->screen->getWidth();
	return rv;
}

SCRIPT_FUNCTION(eggfunc_getResolutionY) {
	GenericValue rv;
	rv.DoubleVal = core->screen->getHeight();
	return rv;
}

SCRIPT_FUNCTION(eggfunc_newRenderStack) {
	GenericValue rv;
	rv.PointerVal = new std::vector<Layer *>();
	return rv;
}

SCRIPT_FUNCTION(eggfunc_setRenderStack) {
	core->renderEngine->setRenderStack((RenderStack *)args[0].PointerVal);
	return nullGV;
}

SCRIPT_FUNCTION(eggfunc_getRenderStack) {
	GenericValue rv;
	rv.PointerVal = core->renderEngine->getRenderStack();
	return rv;
}

SCRIPT_FUNCTION(eggfunc_RenderStack_addLayer) {
	RenderStack *stack = (RenderStack *)args[0].PointerVal;
	stack->push_back((Layer *)args[1].PointerVal);
	return nullGV;
}

SCRIPT_FUNCTION(eggfunc_newLayer) {
	GenericValue rv;
	rv.PointerVal = new Layer(args[0].DoubleVal, args[1].DoubleVal, args[2].DoubleVal, args[3].DoubleVal);
	return rv;
}

SCRIPT_FUNCTION(eggfunc_Layer_add) {
	Layer *layer = (Layer *)args[0].PointerVal;
	layer->elements.push_back((RenderObject *)args[1].PointerVal);
	return nullGV;
}

SCRIPT_FUNCTION(eggfunc_newImage) {
	GenericValue rv;
	rv.PointerVal = new RenderImage((char *)args[0].PointerVal);
	return rv;
}

SCRIPT_FUNCTION(eggfunc_Image_width) {
	GenericValue rv;
	rv.DoubleVal = ((RenderImage *)args[0].PointerVal)->getWidth();
	return rv;
}

SCRIPT_FUNCTION(eggfunc_Image_height) {
	GenericValue rv;
	rv.DoubleVal = ((RenderImage *)args[0].PointerVal)->getHeight();
	return rv;
}

SCRIPT_FUNCTION(eggfunc_newSprite) {
	GenericValue rv;
	rv.PointerVal = new Sprite((RenderImage *)args[0].PointerVal, args[1].DoubleVal, args[2].DoubleVal, args[3].DoubleVal, args[4].DoubleVal);
	return rv;
}

SCRIPT_FUNCTION(eggfunc_Sprite_getX) {
	GenericValue rv;
	Sprite *s = (Sprite *)args[0].PointerVal;
	rv.DoubleVal = s->_x;
	return rv;
}

SCRIPT_FUNCTION(eggfunc_Sprite_getY) {
	GenericValue rv;
	Sprite *s = (Sprite *)args[0].PointerVal;
	rv.DoubleVal = s->_y;
	return rv;
}

SCRIPT_FUNCTION(eggfunc_Sprite_setXY) {
	Sprite *s = (Sprite *)args[0].PointerVal;
	s->_x = args[1].DoubleVal;
	s->_y = args[2].DoubleVal;
	return nullGV;
}

// ---

void ScriptEngine::registerEggLibraryFunctions() {
	registerFunction(testCall, "testCall", VoidType);
	registerFunction(writeStringToLog, "log", VoidType, 1, PointerType);
	registerFunction(writeNumberToLog, "logNumber", VoidType, 1, NumberType);
	registerFunction(addOne, "addOne", NumberType, 1, NumberType);

	registerFunction(eggfunc_delete, "delete", VoidType, 1, PointerType);

	registerFunction(eggfunc_getResolutionX, "getResolutionX", NumberType);
	registerFunction(eggfunc_getResolutionY, "getResolutionY", NumberType);

	registerFunction(eggfunc_getRenderStack, "getRenderStack", PointerType);
	registerFunction(eggfunc_setRenderStack, "setRenderStack", VoidType, 1, PointerType);
	registerFunction(eggfunc_newRenderStack, "newRenderStack", PointerType);
	registerFunction(eggfunc_RenderStack_addLayer, "RenderStack_addLayer", VoidType, 2, PointerType, PointerType);

	registerFunction(eggfunc_newLayer, "newLayer", PointerType, 4, NumberType, NumberType, NumberType, NumberType);
	registerFunction(eggfunc_Layer_add, "Layer_add", VoidType, 2, PointerType, PointerType);

	registerFunction(eggfunc_newImage, "newImage", PointerType, 1, PointerType);
	registerFunction(eggfunc_Image_width, "Image_width", NumberType, 1, PointerType);
	registerFunction(eggfunc_Image_height, "Image_height", NumberType, 1, PointerType);

	registerFunction(eggfunc_newSprite, "newSprite", PointerType, 5, PointerType, NumberType, NumberType, NumberType, NumberType);
	registerFunction(eggfunc_Sprite_getX, "Sprite_getX", NumberType, 1, PointerType);
	registerFunction(eggfunc_Sprite_getY, "Sprite_getY", NumberType, 1, PointerType);
	registerFunction(eggfunc_Sprite_setXY, "Sprite_setXY", VoidType, 3, PointerType, NumberType, NumberType);
}