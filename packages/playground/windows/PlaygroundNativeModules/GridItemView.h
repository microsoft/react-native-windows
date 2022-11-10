// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "GridItemView.g.h"
#include "NativeModules.h"
#include "winrt/Microsoft.ReactNative.h"

namespace winrt::PlaygroundNativeModules::implementation {

class GridItemView : public GridItemViewT<GridItemView> {
  using Super = GridItemViewT<GridItemView>;

 public:
  GridItemView(Microsoft::ReactNative::IReactContext const &reactContext) : m_reactContext{reactContext} {}
  virtual winrt::Windows::Foundation::Size ArrangeOverride(winrt::Windows::Foundation::Size availableSize);

 private:
  Microsoft::ReactNative::IReactContext m_reactContext{nullptr};
};
} // namespace winrt::PlaygroundNativeModules::implementation

namespace winrt::PlaygroundNativeModules::factory_implementation {
struct GridItemView : GridItemViewT<GridItemView, implementation::GridItemView> {};
} // namespace winrt::PlaygroundNativeModules::factory_implementation
