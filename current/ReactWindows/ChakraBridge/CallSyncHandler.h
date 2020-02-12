// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "pch.h"

using namespace Platform;

namespace ChakraBridge {

public
delegate ChakraStringResult
CallSyncHandler(int moduleId, int methodId, String ^ args);

} // namespace ChakraBridge
