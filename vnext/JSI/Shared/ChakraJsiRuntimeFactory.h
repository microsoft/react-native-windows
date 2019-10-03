// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <jsi/jsi.h>

namespace facebook {
namespace jsi {
namespace chakraruntime {

struct ChakraJsiRuntimeArgs;

std::unique_ptr<jsi::Runtime> makeChakraJsiRuntime(
    ChakraJsiRuntimeArgs &&args) noexcept;

} // namespace chakraruntime
} // namespace jsi
} // namespace facebook
