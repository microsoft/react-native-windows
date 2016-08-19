#pragma once

#include "pch.h"
#include <jsrt.h>

class ChakraHost
{
public:
	JsErrorCode Init();
	JsErrorCode Destroy();

	JsErrorCode SerializeScript(const wchar_t* szScript, const wchar_t* szDestination);
	JsErrorCode SerializeScriptFromFile(const wchar_t* szPath, const wchar_t* szDestination);

	JsErrorCode RunSerailizedScript(BYTE* buffer, const wchar_t* szPath, const wchar_t* szSourceUri, JsValueRef* result);
	JsErrorCode RunSerializedScriptFromFile(const wchar_t* szSerializedPath, const wchar_t* szPath, const wchar_t* szSourceUri, JsValueRef* result);

	JsErrorCode RunScript(const wchar_t* szScript, const wchar_t* szSourceUri, JsValueRef* result);
	JsErrorCode RunScriptFromFile(const wchar_t* szPath, const wchar_t* szSourceUri, JsValueRef* result);

	JsErrorCode JsonStringify(JsValueRef argument, JsValueRef* result);
	JsErrorCode JsonParse(JsValueRef argument, JsValueRef* result);

	JsErrorCode GetGlobalVariable(const wchar_t* szPropertyName, JsValueRef* result);
	JsErrorCode SetGlobalVariable(const wchar_t* szPropertyName, JsValueRef value);

	JsValueRef globalObject;
private:
	JsErrorCode InitJson();
	JsErrorCode InitConsole();

	unsigned currentSourceContext;
	JsRuntimeHandle runtime;
	JsContextRef context;
	JsValueRef jsonParseObject;
	JsValueRef jsonStringifyObject;
};