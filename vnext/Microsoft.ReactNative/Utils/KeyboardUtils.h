// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.System.h>

namespace Microsoft::ReactNative {

bool IsModifiedKeyPressed(winrt::CoreWindow const &coreWindow, winrt::Windows::System::VirtualKey virtualKey);
std::string FromVirtualKey(winrt::Windows::System::VirtualKey virtualKey, bool fShift, bool fCaps);
std::string CodeFromVirtualKey(winrt::Windows::System::VirtualKey virtualKey);

} // Microsoft::ReactNative