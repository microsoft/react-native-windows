// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#define NOGDI
#define NOMINMAX

#include <unknwn.h>

#undef GetCurrentTime

#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>

#include "gtest/gtest.h"
#include "motifCpp/gTestAdapter.h"
#include "motifCpp/testCheck.h"

#ifndef CXXUNITTESTS
#define CXXUNITTESTS
#endif // CXXUNITTESTS
