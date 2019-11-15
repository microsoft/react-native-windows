// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#ifdef CHAKRACORE

#include "ChakraCoreRuntimeFactory.h"

#include "ChakraCoreRuntime.h"

namespace Microsoft::JSI {

std::unique_ptr<facebook::jsi::Runtime> makeChakraCoreRuntime(std::shared_ptr<ChakraCoreRuntimeArgs> &&args) noexcept {
  auto runtime = std::make_unique<ChakraCoreRuntime>(std::move(args));
  runtime->Initialize();
  return runtime;
}

} // namespace Microsoft::JSI

#endif
