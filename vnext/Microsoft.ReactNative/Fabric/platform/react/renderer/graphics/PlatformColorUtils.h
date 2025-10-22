// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.UI.h>

// Forward declaration to avoid circular dependencies
namespace facebook::react {
class SharedColor;
}

namespace facebook::react {

winrt::Windows::UI::Color ResolvePlatformColor(const std::vector<std::string> &semanticItems);

// Get appropriate placeholder text color for TextInput based on focus state and background
SharedColor GetTextInputPlaceholderColor(bool isFocused, const winrt::Windows::UI::Color &backgroundColor = {});

} // namespace facebook::react
