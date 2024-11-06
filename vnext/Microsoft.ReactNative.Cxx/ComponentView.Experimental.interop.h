// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

namespace Microsoft::ReactNative::Composition::Experimental {

// This interface will be removed once we complete internal transitions to ContentIsland, and new APIs for hosting
// popups/new windows from a ContentIsland
struct __declspec(uuid("2A60D68A-57B8-43CA-B1BA-2FAF0FCB344B")) IComponentViewInterop : IUnknown {
  virtual HWND GetHwndForParenting() noexcept = 0;
};

} // namespace Microsoft::ReactNative::Composition::Experimental
