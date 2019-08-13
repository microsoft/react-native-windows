// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#ifdef CHAKRACORE

#include "ChakraCoreRuntimeFactory.h"

#include "ChakraCoreRuntime.h"

namespace facebook::jsi::chakra {

std::unique_ptr<Runtime> makeChakraCoreRuntime(ChakraRuntimeArgs &&args) {
  return std::make_unique<ChakraCoreRuntime>(std::move(args));
}

} // namespace facebook::jsi::chakra

#endif
