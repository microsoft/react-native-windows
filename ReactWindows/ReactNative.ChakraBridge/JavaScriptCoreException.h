#pragma once

using namespace Platform;

namespace ReactNative { namespace ChakraBridge {

public ref class JavaScriptCoreException : Exception
{
public:
	JavaScriptCoreException(int jsErrorCode) : m_jsErrorCode(jsErrorCode), Exception(0, ref new String(L"A fatal exception has occurred in a JavaScript runtime"))
	{

	}

	JavaScriptCoreException(int jsErrorCode, String^ message) : m_jsErrorCode(jsErrorCode), Exception(0, message) 
	{ 
	}

	property int JsErrorCode
	{
		int get() { return m_jsErrorCode; }
	}
private:
	int m_jsErrorCode;
};

};};