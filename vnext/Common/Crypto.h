// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <array>

namespace Microsoft::Common::Crypto {

using Sha256Hash = std::array<uint8_t, 32>;
Sha256Hash calculateSha256Hash(const void *data, size_t bufferSize);

} // namespace Microsoft::Common::Crypto
