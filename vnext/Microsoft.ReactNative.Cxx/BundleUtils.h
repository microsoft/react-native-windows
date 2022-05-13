// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <winrt/base.h>

namespace Microsoft::ReactNative {
winrt::array_view<char> GetEmbeddedResource(const winrt::hstring &str);
}
