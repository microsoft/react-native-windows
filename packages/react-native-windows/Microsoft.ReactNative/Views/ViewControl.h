// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/ViewPanel.h>

#include "ViewControl.g.h"

namespace winrt::Microsoft::ReactNative::implementation {

//
// ViewControl is a ContentControl that ViewViewManager uses to wrap a ViewPanel
// when we want that ViewPanel to be keyboard focusable
//
struct ViewControl : ViewControlT<ViewControl> {
  using Super = ViewControlT<ViewControl>;

 public:
  // Constructors
  ViewControl();

  xaml::Automation::Peers::AutomationPeer OnCreateAutomationPeer();

  winrt::Microsoft::ReactNative::ViewPanel GetPanel() const;
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct ViewControl : ViewControlT<ViewControl, implementation::ViewControl> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
