// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

namespace Microsoft::React::Networking {

enum class OriginPolicy : size_t {
  None = 0,
  SingleOrigin = 1,
  SimpleCrossOriginResourceSharing = 2,
  CrossOriginResourceSharing = 3,
};

}//namespace
