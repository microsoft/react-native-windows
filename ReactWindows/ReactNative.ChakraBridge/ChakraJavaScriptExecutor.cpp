#include "pch.h"
#include "ChakraJavaScriptExecutor.h"

using namespace ReactNative::ChakraBridge;

ChakraJavaScriptExecutor::ChakraJavaScriptExecutor()
{
	this->host.Init();
}

ChakraJavaScriptExecutor::~ChakraJavaScriptExecutor()
{
	this->host.Destroy();
}

void ChakraJavaScriptExecutor::SetGlobalVariable(String^ variableName, JsonValue^ value)
{
	JsErrorCode status = JsNoError;
	
	JsValueRef valueStringified;
	String^ valueText = value->Stringify();
	status = JsPointerToString(valueText->Data(), valueText->Length(), &valueStringified);
	if (status != JsNoError)
	{
		// TODO: Throw right error
	}

	JsValueRef valueJson;
	status = this->host.JsonParse(valueStringified, &valueJson);
	if (status != JsNoError)
	{
		// TODO: Throw right error
	}

	status = this->host.SetGlobalVariable(variableName->Data(), valueJson);
	if (status != JsNoError)
	{
		// TODO: Throw right error
	}
}

JsonValue^ ChakraJavaScriptExecutor::GetGlobalVariable(String^ variableName)
{
	JsErrorCode status = JsNoError;

	JsValueRef globalVariable;
	status = this->host.GetGlobalVariable(variableName->Data(), &globalVariable);
	if (status != JsNoError)
	{
		// TODO: Throw right error
	}

	JsValueRef globalVariableJson;
	status = this->host.JsonStringify(globalVariable, &globalVariableJson);
	if (status != JsNoError)
	{
		// TODO: Throw right error
	}

	const wchar_t* szBuf;
	size_t bufLen;
	status = JsStringToPointer(globalVariableJson, &szBuf, &bufLen);
	if (status != JsNoError)
	{
		// TODO: Throw right error
	}

	return JsonValue::Parse(ref new String(szBuf, bufLen));
}