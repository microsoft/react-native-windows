// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#ifndef CHAKRACORE

#include "ChakraRtRuntimeFactory.h"

#include "ChakraRtRuntime.h"
#include "ChakraRuntimeArgs.h"

namespace Microsoft::JSI {

std::unique_ptr<facebook::jsi::Runtime> makeChakraRtRuntime(std::shared_ptr<ChakraRtRuntimeArgs> &&args) noexcept {
  auto runtime = std::make_unique<ChakraRtRuntime>(std::move(args));
  runtime->Initialize();
  return runtime;
}

} // namespace Microsoft::JSI

#endif
