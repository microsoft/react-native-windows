#pragma once

#include <jsrt.h>

struct SerializedSourceContext
{
	BYTE* byteCode;
	const wchar_t* sourcePath;
	wchar_t* scriptBuffer;

	~SerializedSourceContext()
	{
		delete[] byteCode;
		delete[] sourcePath;
		if (scriptBuffer)
		{
			delete[] scriptBuffer;
		}
	}
};