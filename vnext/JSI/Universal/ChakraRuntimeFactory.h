// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#ifndef CHAKRACORE

#include "ChakraRuntimeArgs.h"

#include <memory>

namespace facebook::jsi {

class Runtime;

namespace chakra {

std::unique_ptr<Runtime> makeChakraRuntime(ChakraRuntimeArgs &&args);

} // namespace chakra
} // namespace facebook::jsi

#endif
