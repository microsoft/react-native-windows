#pragma once

#include "pch.h"
#include <jsrt.h>

class ChakraHost
{
public:
	JsErrorCode Init();
	JsErrorCode Destroy();

	JsErrorCode SerializeScript(const wchar_t* szScript, const wchar_t* szDestination);

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

/*
struct MySourceContext
{
~MySourceContext()
{
delete [] byteCode;
delete [] sourcePath;
if (scriptBuffer)
{
delete [] scriptBuffer;
}
}
char * byteCode;
char * sourcePath;
wchar_t * scriptBuffer;
};
bool JsSerializedScriptLoadSourceCallback(JsSourceContext sourceContext, wchar_t ** scriptBuffer)
{
MySourceContext * mySourceContext = (MySourceContext *)sourceContext;
*scriptBuffer = ReadSource(mySourceContext->sourcePath);
return true;
}

void JsSerializedScriptUnloadCallback(JsSourceContext sourceContext)
{
MySourceContext * mySourceContext = (MySourceContext *)sourceContext;
delete mySourceContext;
}

void RunScript(char * byteCode, char * sourcePath)
{

MySourceContext * sourceContext = new MySourceContext;
sourceContext->byteCode = byteCode;
sourceContext->sourcePath = sourcePath;
sourceContext->scriptBuffer = nullptr;
JsValueRef result;
JsRunSerializeScriptWithCallBack(&JsSerializedScriptLoadSourceCallback, &JsSerializedScriptUnloadCallback, byteCode, sourceContext, sourcePath, &result);
}
*/