// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.UI.h>

namespace facebook::react {

winrt::Windows::UI::Color ResolvePlatformColor(const std::vector<std::string> &semanticItems);

} // namespace facebook::react
