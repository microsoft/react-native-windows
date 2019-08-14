// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "ChakraCoreRuntimeExFactory.h"

#include "ChakraCoreRuntimeEx.h"

namespace facebook::jsi::chakra {

std::unique_ptr<Runtime> makeChakraCoreRuntimeEx(
    ChakraRuntimeArgs &&args,
    ChakraCoreRuntimeExArgs &&officeArgs) {
  return std::make_unique<ChakraCoreRuntimeEx>(
      std::move(args), std::move(officeArgs));
}

} // namespace facebook::jsi::chakra
