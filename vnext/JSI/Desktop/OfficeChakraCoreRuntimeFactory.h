// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ChakraRuntimeArgs.h"
#include "OfficeChakraCoreRuntimeArgs.h"

#include <memory>

namespace facebook::jsi {

class Runtime;

namespace chakra {

std::unique_ptr<Runtime> makeOfficeChakraCoreRuntime(
    ChakraRuntimeArgs &&args,
    OfficeChakraCoreRuntimeArgs &&officeArgs);

} // namespace chakra
} // namespace facebook::jsi
