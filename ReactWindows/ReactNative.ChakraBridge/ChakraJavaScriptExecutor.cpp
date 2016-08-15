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