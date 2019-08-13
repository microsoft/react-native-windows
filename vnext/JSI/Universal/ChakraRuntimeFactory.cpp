// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#ifndef CHAKRACORE

#include "ChakraRuntimeFactory.h"

#include "ChakraRuntime.h"

namespace facebook::jsi::chakra {

std::unique_ptr<Runtime> makeChakraRuntime(ChakraRuntimeArgs &&args) {
  return std::make_unique<ChakraRuntime>(std::move(args));
}

} // namespace facebook::jsi::chakra

#endif
