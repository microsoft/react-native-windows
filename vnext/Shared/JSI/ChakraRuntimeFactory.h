// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <jsi/jsi.h>

namespace Microsoft::JSI {

struct ChakraRuntimeArgs;

std::unique_ptr<facebook::jsi::Runtime> makeChakraRuntime(ChakraRuntimeArgs &&args) noexcept;

} // namespace Microsoft::JSI
