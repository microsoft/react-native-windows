// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#ifndef CHAKRACORE

#include <memory>

namespace facebook::jsi {
class Runtime;
}

namespace Microsoft::JSI {

struct ChakraRuntimeArgs;
using ChakraRtRuntimeArgs = ChakraRuntimeArgs;

std::unique_ptr<facebook::jsi::Runtime> makeChakraRtRuntime(std::shared_ptr<ChakraRtRuntimeArgs> &&args) noexcept;

} // namespace Microsoft::JSI

#endif
