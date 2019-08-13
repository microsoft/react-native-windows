// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "OfficeChakraCoreRuntimeFactory.h"

#include "OfficeChakraCoreRuntime.h"

namespace facebook::jsi::chakra {

std::unique_ptr<Runtime> makeOfficeChakraCoreRuntime(
    ChakraRuntimeArgs &&args,
    OfficeChakraCoreRuntimeArgs &&officeArgs) {
  return std::make_unique<OfficeChakraCoreRuntime>(
      std::move(args), std::move(officeArgs));
}

} // namespace facebook::jsi::chakra
