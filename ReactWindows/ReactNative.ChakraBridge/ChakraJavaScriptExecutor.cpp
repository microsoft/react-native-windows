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

void ChakraJavaScriptExecutor::SetGlobalVariable(String^ variableName, String^ stringifiedText)
{
	JsErrorCode status = JsNoError;
	
	JsValueRef valueStringified;
	status = JsPointerToString(stringifiedText->Data(), stringifiedText->Length(), &valueStringified);
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

String^ ChakraJavaScriptExecutor::GetGlobalVariable(String^ variableName)
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

	return ref new String(szBuf, bufLen);
}

String^ ChakraJavaScriptExecutor::RunScript(String^ source, String^ sourceUri)
{
	JsErrorCode status = JsNoError;

	JsValueRef result;
	status = this->host.RunScript(source->Data(), sourceUri->Data(), &result);
	if (status != JsNoError)
	{
		// TODO: Throw right error
	}

	JsValueRef resultJson;
	status = this->host.JsonStringify(result, &resultJson);
	if (status != JsNoError)
	{
		// TODO: Throw right error
	}

	const wchar_t* szBuf;
	size_t bufLen;
	status = JsStringToPointer(resultJson, &szBuf, &bufLen);
	if (status != JsNoError)
	{
		// TODO: Throw right error
	}

	return ref new String(szBuf, bufLen);
}

IAsyncOperation<String^>^ ChakraJavaScriptExecutor::RunScriptFromFile(String^ sourceUri)
{
	Uri^ fileUri = ref new Uri(sourceUri);
	return create_async([this, fileUri, sourceUri]
	{
		return create_task(StorageFile::GetFileFromApplicationUriAsync(fileUri))
			.then([this, sourceUri](StorageFile^ storageFile)
			{
				return create_task(FileIO::ReadTextAsync(storageFile));
			})
			.then([this, sourceUri](String^ str)
			{
				return create_async([this, sourceUri, str] { return this->RunScript(str, sourceUri); });
			});
	});
}