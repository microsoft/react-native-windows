// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <jsi/jsi.h>

namespace facebook {
namespace jsi {
namespace chakraruntime {

struct ChakraRuntimeArgs;

std::unique_ptr<jsi::Runtime> makeChakraRuntime(
    ChakraRuntimeArgs &&args) noexcept;

} // namespace chakraruntime
} // namespace jsi
} // namespace facebook
