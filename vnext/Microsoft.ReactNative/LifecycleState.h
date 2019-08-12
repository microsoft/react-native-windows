#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace winrt::Microsoft::ReactNative {
// An enumeration to signify the current lifecycle state for a
// ReactInstanceManager.
enum struct LifecycleState {
  // Lifecycle state before an application is initially created.
  BeforeCreate,

  // Lifecycle state before an application is resumed.
  Suspended,

  // Lifecycle state of a foreground running application.
  Foreground,

  // Lifecycle state when the application is in the background.
  Background,
};
} // namespace winrt::Microsoft::ReactNative
