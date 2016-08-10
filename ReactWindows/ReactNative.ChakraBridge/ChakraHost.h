#pragma once

#include "pch.h"
#define USE_EDGEMODE_JSRT
#include <jsrt.h>

namespace ReactNative {
namespace ChakraBridge {

class ChakraHost
{
public:
	JsErrorCode _cdecl Init(bool bDebug);
	JsErrorCode _cdecl Destroy();
	JsValueRef _cdecl RunScriptFromFile(const wchar_t* fileName);
	//JsValueRef _cdecl RunScript(const wchar_t* script);

private:
	JsRuntimeHandle runtime;
	JsContextRef context;
	unsigned currentSourceContext;
};

};};