// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.UI.h>

namespace facebook::react {

winrt::Windows::UI::Color ResolvePlatformColor(const std::vector<std::string> &semanticItems);

// Get appropriate placeholder text color for TextInput based on focus state and background
winrt::Windows::UI::Color GetTextInputPlaceholderColor(
    bool isFocused,
    const winrt::Windows::UI::Color &backgroundColor = {});

} // namespace facebook::react
