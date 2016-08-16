#pragma once

using namespace Platform;

namespace ReactNative { namespace ChakraBridge {

public value struct ChakraStringResult
{
	int ErrorCode;
	String^ Result;
};

};};