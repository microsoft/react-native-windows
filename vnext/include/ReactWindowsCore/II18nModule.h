// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/CxxModule.h>

namespace react { namespace windows {

struct II18nModule
{
	virtual ~II18nModule() {};
	virtual std::string getLocaleIdentifier() = 0;
	virtual bool getIsRTL() = 0;
};

std::unique_ptr<facebook::xplat::module::CxxModule> createI18nModule(std::unique_ptr<II18nModule> module);
} } // react::windows