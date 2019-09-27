// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <stdint.h>
#include "JsModulesUnbundle.h"

class JsIndexedModulesUnbundle : public JsModulesUnbundle
{
public:
    JsIndexedModulesUnbundle(const wchar_t* szSourcePath);
	~JsIndexedModulesUnbundle();
	virtual JsModulesUnbundleModule* GetModule(uint32_t index) override;
	virtual JsErrorCode GetStartupCode(wchar_t** pszScript) override;
private:
    wchar_t* sourcePath;
	HANDLE fileHandle;
	HANDLE mapHandle;
	BYTE* byteBuffer;

	struct ModuleData {
		uint32_t offset;
		uint32_t length;
	};

	static_assert(
		sizeof(ModuleData) == 8,
		"ModuleData must not have any padding and use sizes matching input files");

	struct ModuleTable {
		size_t numEntries;
		ModuleData* modules;
		ModuleTable() : numEntries(0) {};
		ModuleTable(size_t entries) :
			numEntries(entries),
			modules(new ModuleData[numEntries]) {};
		size_t byteLength() const {
			return numEntries * sizeof(ModuleData);
		}
	};

	ModuleTable moduleTable;
	uint32_t baseOffset;
};

