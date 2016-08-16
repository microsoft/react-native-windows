#pragma once

using namespace Platform;

namespace ReactNative { namespace ChakraBridge {

public ref class JavaScriptNativeException sealed : Exception
{
public:
	JavaScriptNativeException(int code) : m_code(code), Exception(L"") { }

private:
	int m_code;
};

};};