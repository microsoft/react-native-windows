// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ChakraHost.h"
#include "JsIndexedModulesUnbundle.h"

JsIndexedModulesUnbundle::JsIndexedModulesUnbundle(const wchar_t* szSourcePath)
{
	// Clone the source path in case it is garbage collected
	sourcePath = new wchar_t[_MAX_DIR];
	sourcePath[0] = L'\0';
	wcscat_s(sourcePath, _MAX_DIR, szSourcePath);
}

JsIndexedModulesUnbundle::~JsIndexedModulesUnbundle()
{
	delete[] sourcePath;

	if (fileHandle != NULL)
	{
		UnmapViewOfFile(byteBuffer);
		CloseHandle(mapHandle);
		CloseHandle(fileHandle);
	}
	else
	{
		delete[] byteBuffer;
	}
}

JsModulesUnbundleModule* JsIndexedModulesUnbundle::GetModule(uint32_t index)
{
	// The module data is little-endian, this function assumes a little-endian host
	ModuleData moduleData = moduleTable.modules[index];

	auto module = new JsModulesUnbundleModule();
	auto filename = new wchar_t[_MAX_FNAME];
	auto script = new char[moduleData.length];
	auto wcScript = new wchar_t[moduleData.length];

	// Create the source URL
	IfErrnoCleanup(_itow_s(index, filename, _MAX_FNAME, 10));
	IfErrnoCleanup(wcscat_s(filename, _MAX_FNAME, L".js"));

	// Copy the script
	size_t numConverted;
	IfErrnoCleanup(memcpy_s(script, moduleData.length, byteBuffer + baseOffset + moduleData.offset, moduleData.length));
	IfErrnoCleanup(mbstowcs_s(&numConverted, wcScript, moduleData.length, script, moduleData.length));
	delete[] script;

	// Return the module
	module->source = wcScript;
	module->sourceUrl = filename;
	return module;

cleanup:
	delete module;
	delete[] filename;
	delete[] script;
	delete[] wcScript;
	return nullptr;
}

JsErrorCode JsIndexedModulesUnbundle::GetStartupCode(wchar_t** pszScript)
{
	// Open the memory mapped file
	IfFailRet(ChakraHost::LoadByteCode(sourcePath, &byteBuffer, &fileHandle, &mapHandle, true));

	// Copy the header data
	uint32_t header[3];
	if (memcpy_s(header, sizeof(header), byteBuffer, sizeof(header)))
	{
		return JsErrorFatal;
	}

	// Create the module table
	uint32_t numberOfTableEntries = header[1];
	moduleTable = ModuleTable(numberOfTableEntries);
	uint32_t moduleTableSize = moduleTable.byteLength();
	if (memcpy_s(moduleTable.modules, moduleTableSize, byteBuffer + sizeof(header), moduleTableSize))
	{
		return JsErrorFatal;
	}

	// Store the base offset
	baseOffset = sizeof(header) + moduleTableSize;

	// Copy the startup code
	uint32_t startupCodeSize = header[2];
	auto startupCode = new char[startupCodeSize];
	auto wcStartupCode = new wchar_t[startupCodeSize];
	if (memcpy_s(startupCode, startupCodeSize, byteBuffer + baseOffset, startupCodeSize))
	{
		delete[] startupCode;
		delete[] wcStartupCode;
		return JsErrorFatal;
	}

	// Convert startup code to wide char
	size_t ignored;
	if (mbstowcs_s(&ignored, wcStartupCode, startupCodeSize, startupCode, startupCodeSize))
	{
		delete[] startupCode;
		delete[] wcStartupCode;
		return JsErrorFatal;
	}

	delete[] startupCode;
	*pszScript = wcStartupCode;

	return JsNoError;
}
