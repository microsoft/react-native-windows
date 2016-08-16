#pragma once

#include "pch.h"
#define USE_EDGEMODE_JSRT
#include <jsrt.h>

class ChakraHost
{
public:
	JsErrorCode Init();
	JsErrorCode Destroy();

	JsErrorCode RunScript(const wchar_t* szScript, const wchar_t* szSourceUri, JsValueRef* result);

	JsErrorCode JsonStringify(JsValueRef argument, JsValueRef* result);
	JsErrorCode JsonParse(JsValueRef argument, JsValueRef* result);

	JsErrorCode GetGlobalVariable(const wchar_t* szPropertyName, JsValueRef* result);
	JsErrorCode SetGlobalVariable(const wchar_t* szPropertyName, JsValueRef value);

	JsValueRef globalObject;
	JsValueRef requireObject;
private:
	JsErrorCode InitJson();
	JsErrorCode InitConsole();
	JsErrorCode InitRequire();

	unsigned currentSourceContext;
	JsRuntimeHandle runtime;
	JsContextRef context;
	JsValueRef jsonParseObject;
	JsValueRef jsonStringifyObject;
};
