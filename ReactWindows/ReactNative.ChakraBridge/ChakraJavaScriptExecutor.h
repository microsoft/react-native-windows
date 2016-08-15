#pragma once

#include "pch.h"
#include <jsrt.h>
#include "ChakraHost.h"

using namespace Platform;
using namespace Windows::Data::Json;

namespace ReactNative { namespace ChakraBridge {

[Windows::Foundation::Metadata::WebHostHidden]
public ref class ChakraJavaScriptExecutor sealed
{
public:
	ChakraJavaScriptExecutor();
	virtual ~ChakraJavaScriptExecutor();

	JsonValue^ GetGlobalVariable(String^ variableName);
	void SetGlobalVariable(String^ variableName, JsonValue^ value);
private:
	ChakraHost host;
};

};};
