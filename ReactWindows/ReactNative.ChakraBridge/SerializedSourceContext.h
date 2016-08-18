#pragma once

#include <jsrt.h>

struct SerializedSourceContext
{
	BYTE* serializedBuffer;
	wchar_t* sourcePath;
	wchar_t* scriptBuffer;

	~SerializedSourceContext()
	{
		delete[] serializedBuffer;
		delete[] sourcePath;
		if (scriptBuffer)
		{
			delete[] scriptBuffer;
		}
	}
};