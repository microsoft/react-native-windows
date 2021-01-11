// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// We include all ReactCommon header files here to be able to disable their warnings in one place.

#include "FollyIncludes.h"

#pragma warning(push)
#pragma warning(disable : 4100) // '<args_0>': unreferenced formal parameter
#include <cxxreact/SystraceSection.h>
#pragma warning(pop)

#include <cxxreact/CxxModule.h>
#include <cxxreact/Instance.h>
#include <cxxreact/JSBigString.h>
#include <cxxreact/JSExecutor.h>
#include <fbsystrace.h>
#include <yoga/yoga.h>
