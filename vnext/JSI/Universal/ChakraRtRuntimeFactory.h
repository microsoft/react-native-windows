// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#ifndef CHAKRACORE

#include "ChakraRuntimeArgs.h"

#include <memory>

namespace facebook::jsi {
class Runtime;
}

namespace Microsoft::JSI {

using ChakraRtRuntimeArgs = ChakraRuntimeArgs;

std::unique_ptr<facebook::jsi::Runtime> makeChakraRtRuntime(const std::shared_ptr<ChakraRtRuntimeArgs> &args) noexcept;

} // namespace Microsoft::JSI

#endif
