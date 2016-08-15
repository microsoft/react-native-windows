#pragma once

#include "pch.h"
#include <jsrt.h>
#include <ppltasks.h>
#include "ChakraHost.h"

using namespace concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;

namespace ReactNative { namespace ChakraBridge {

[Windows::Foundation::Metadata::WebHostHidden]
public ref class ChakraJavaScriptExecutor sealed
{
public:
	ChakraJavaScriptExecutor();
	virtual ~ChakraJavaScriptExecutor();

	String^ GetGlobalVariable(String^ variableName);
	void SetGlobalVariable(String^ variableName, String^ value);

	String^ RunScript(String^ source, String^ sourceUri);
	IAsyncOperation<String^>^ RunScriptFromFile(String^ sourceUri);
private:
	ChakraHost host;
};

};};
