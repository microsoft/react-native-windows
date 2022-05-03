// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/base.h>

namespace Microsoft::ReactNative {
winrt::array_view<char> GetEmbeddedResource(const winrt::hstring &str);
winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::InMemoryRandomAccessStream>
GetImageInlineDataAsync(winrt::hstring uri);
} // namespace Microsoft::ReactNative
