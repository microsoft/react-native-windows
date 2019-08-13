// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace facebook::jsi::chakra {

struct ChakraRuntimeArgs {

  // When this flag is set to true, the runtime will not do any work (such as
  // garbage collection) on background threads.
  bool disableBackgroundWork{false};

  // When this flag is set to true, the runtime will not allocate executable
  // code pages. This also implies that Native Code generation will be turned
  // off. Note that this will break JavaScript stack decoding in tools like WPA,
  // since they rely on allocation of unique thunks to interpret each function
  // and allocation of those thunks will be disabled as well.
  bool disableExecutablePageAllocation{false};
};

} // namespace facebook::jsi::chakra
