// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ViewGrid.g.h"

namespace winrt::Microsoft::ReactNative::implementation {

//
// ViewGrid is our custom Panel used by ViewViewManager
//
struct ViewGrid : ViewGridT<ViewGrid> {
  using Super = ViewGridT<ViewGrid>;

 public:
  // Constructors
  ViewGrid();

  xaml::Automation::Peers::AutomationPeer OnCreateAutomationPeer();

  // Overrides
  virtual winrt::Windows::Foundation::Size MeasureOverride(winrt::Windows::Foundation::Size availableSize);
  virtual winrt::Windows::Foundation::Size ArrangeOverride(winrt::Windows::Foundation::Size finalSize);
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct ViewGrid : ViewGridT<ViewGrid, implementation::ViewGrid> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation

namespace Microsoft::ReactNative {
// Issue #2172: Calling static members on winrt::Microsoft::ReactNative::ViewPanel fails to
// call
// down into winrt::Microsoft::ReactNative::implementation::ViewPanel because of how we're
// using cppwinrt. This workaround is so that consumers in Microsoft::ReactNative can just
// call ViewPanel

using ViewGrid = winrt::Microsoft::ReactNative::implementation::ViewGrid;
} // namespace Microsoft::ReactNative
