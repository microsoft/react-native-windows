// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#ifdef CHAKRACORE

#include "ChakraCoreRuntimeArgs.h"

#include <memory>

namespace facebook::jsi {
class Runtime;
}

namespace Microsoft::JSI {

std::unique_ptr<facebook::jsi::Runtime> makeChakraCoreRuntime(std::shared_ptr<ChakraCoreRuntimeArgs> &&args) noexcept;

} // namespace Microsoft::JSI

#endif
