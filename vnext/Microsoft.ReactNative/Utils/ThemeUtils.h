// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.UI.h>

namespace Microsoft::ReactNative {

bool IsColorLight(const winrt::Windows::UI::Color &clr) noexcept;
bool IsInHighContrastWin32() noexcept;

} // namespace Microsoft::ReactNative
