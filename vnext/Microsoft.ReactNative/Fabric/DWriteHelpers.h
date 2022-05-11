
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <dwrite.h>

namespace Microsoft::ReactNative {

winrt::com_ptr<::IDWriteFactory> DWriteFactory() noexcept;

} // namespace Microsoft::ReactNative
