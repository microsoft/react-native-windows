// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// We include all ReactCommon header files here to be able to disable their warnings in one place.

#include "FollyIncludes.h"

#pragma warning(push)
#pragma warning(disable : 4996) // deprecated APIs in cxxreact headers

#include <cxxreact/JSBigString.h>
#include <cxxreact/JSExecutor.h>
#include <cxxreact/MessageQueueThread.h>
#include <cxxreact/ModuleRegistry.h>
#include <cxxreact/NativeModule.h>
#include <cxxreact/RAMBundleRegistry.h>
#include <fbsystrace.h>
#include <jsiexecutor/jsireact/JSIExecutor.h>
#include <jsiexecutor/jsireact/JSINativeModules.h>
#include <yoga/Yoga.h>

#include <ReactCommon/TurboModule.h>

#pragma warning(pop)
