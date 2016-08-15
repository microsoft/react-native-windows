#pragma once

#include "pch.h"
#include <jsrt.h>
#include "ChakraHost.h"

namespace ReactNative { namespace ChakraBridge {

	public ref class ChakraJavaScriptExecutor sealed
{
public:
	ChakraJavaScriptExecutor();
	virtual ~ChakraJavaScriptExecutor();
private:
	ChakraHost host;
};

};};
