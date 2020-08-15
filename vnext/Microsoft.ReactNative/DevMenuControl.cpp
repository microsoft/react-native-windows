// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "DevMenuControl.h"
#if __has_include("DevMenuControl.g.cpp")
#include "DevMenuControl.g.cpp"
#endif

namespace winrt::Microsoft::ReactNative::implementation {
DevMenuControl::DevMenuControl() {
  InitializeComponent();
}
} // namespace winrt::Microsoft::ReactNative::implementation
