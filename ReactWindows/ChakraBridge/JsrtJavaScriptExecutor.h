#pragma once

#include "pch.h"
#include <jsrt.h>
#include <ppltasks.h>
#include "ChakraHost.h"
#include "ChakraStringResult.h"

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;

namespace ChakraBridge {

public ref class JsrtJavaScriptExectutor sealed
{
public:
    int InitializeHost();
    int DisposeHost();

    ChakraStringResult GetGlobalVariable(String^ variableName);
    int SetGlobalVariable(String^ variableName, String^ value);

    ChakraStringResult RunScript(String^ source, String^ sourceUri);
    ChakraStringResult RunScriptFromFile(String^ sourceFilePath, String^ sourceUri);

	int SerializeScript(String^ source, String^ destination);
	int SerializeScriptFromFile(String^ file, String^ destination);

    ChakraStringResult CallFunctionAndReturnFlushedQueue(String^ moduleName, String^ methodName, String^ args); 
    ChakraStringResult InvokeCallbackAndReturnFlushedQueue(int callbackId, String^ args); 
    ChakraStringResult FlushedQueue(); 
private:
    ChakraHost host;
};

};
