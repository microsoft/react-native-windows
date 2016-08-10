#include "pch.h"
#include "ChakraHost.h"
#include <string>
#include <iostream>

using namespace std;
using namespace ReactNative::ChakraBridge;

wstring LoadScript(const wchar_t* szFileName)
{
	FILE *file;
	if (_wfopen_s(&file, szFileName, L"rb"))
	{
		fwprintf(stderr, L"chakrahost: unable to open file: %s.\n", szFileName);
		return wstring();
	}

	unsigned int current = ftell(file);
	fseek(file, 0, SEEK_END);
	unsigned int end = ftell(file);
	fseek(file, current, SEEK_SET);
	unsigned int lengthBytes = end - current;
	char *rawBytes = (char *)calloc(lengthBytes + 1, sizeof(char));

	if (rawBytes == nullptr)
	{
		fwprintf(stderr, L"chakrahost: fatal error.\n");
		return wstring();
	}

	fread((void *)rawBytes, sizeof(char), lengthBytes, file);

	wchar_t *contents = (wchar_t *)calloc(lengthBytes + 1, sizeof(wchar_t));
	if (contents == nullptr)
	{
		free(rawBytes);
		fwprintf(stderr, L"chakrahost: fatal error.\n");
		return wstring();
	}

	if (MultiByteToWideChar(CP_UTF8, 0, rawBytes, lengthBytes + 1, contents, lengthBytes + 1) == 0)
	{
		free(rawBytes);
		free(contents);
		fwprintf(stderr, L"chakrahost: fatal error.\n");
		return wstring();
	}

	wstring result = contents;
	free(rawBytes);
	free(contents);
	return result;
}

JsErrorCode ChakraHost::Init(bool bDebug)
{
	currentSourceContext = 0;

	IfFailRet(JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &runtime));
	IfFailRet(JsCreateContext(runtime, &context));
	IfFailRet(JsSetCurrentContext(context));
	IfFailRet(JsProjectWinRTNamespace(L"Windows"));

	return JsNoError;
};

JsErrorCode ChakraHost::Destroy()
{
	IfFailRet(JsSetCurrentContext(JS_INVALID_REFERENCE));
	IfFailRet(JsDisposeRuntime(runtime));

	return JsNoError;
};

JsValueRef ChakraHost::RunScriptFromFile(const wchar_t* szFileName)
{
	JsValueRef result = JS_INVALID_REFERENCE;

	wstring contents = LoadScript(szFileName);
	if (contents.empty()) 
	{
		return result;
	}

	JsRunScript(contents.c_str(), currentSourceContext++, szFileName, &result);

	return result;
}