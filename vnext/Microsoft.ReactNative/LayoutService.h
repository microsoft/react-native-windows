// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "LayoutService.g.h"
#include "INativeUIManager.h"
#include "ReactHost/React.h"
#include "ReactPropertyBag.h"
#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative::implementation {
struct LayoutService : LayoutServiceT<LayoutService> {
 public:
  LayoutService(Mso::CntPtr<Mso::React::IReactContext> &&context) noexcept;
  static winrt::Microsoft::ReactNative::LayoutService FromContext(IReactContext context);
  static ReactPropertyId<LayoutService> LayoutServiceProperty() noexcept;

  void ApplyLayoutForAllNodes() noexcept;
  void ApplyLayout(int64_t reactTag, float width, float height) noexcept;
  bool IsInBatch() noexcept;
  void MarkDirty(int64_t reactTag) noexcept;

 private:
  Mso::CntPtr<Mso::React::IReactContext> m_context;
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct LayoutService : LayoutServiceT<LayoutService, implementation::LayoutService> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
