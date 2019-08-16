// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "pch.h"

using namespace Platform;

namespace ChakraBridge {

public
delegate ChakraStringResult
CallSyncHandler(int moduleId, int methodId, String ^ args);

} // namespace ChakraBridge
