// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ChakraCoreRuntimeExArgs.h"
#include "ChakraRuntimeArgs.h"

#include <memory>

namespace facebook::jsi {

class Runtime;

namespace chakra {

std::unique_ptr<Runtime> makeChakraCoreRuntimeEx(
    ChakraRuntimeArgs &&args,
    ChakraCoreRuntimeExArgs &&officeArgs);

} // namespace chakra
} // namespace facebook::jsi
