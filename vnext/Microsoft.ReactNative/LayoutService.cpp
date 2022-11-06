// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "LayoutService.h"
#include "LayoutService.g.cpp"
#include <Modules/NativeUIManager.h>
#include <Modules/PaperUIManagerModule.h>

namespace winrt::Microsoft::ReactNative::implementation {

LayoutService::LayoutService(Mso::CntPtr<Mso::React::IReactContext> &&context) noexcept : m_context(context) {}

/*static*/ winrt::Microsoft::ReactNative::LayoutService LayoutService::FromContext(IReactContext context) {
  return context.Properties()
      .Get(LayoutService::LayoutServiceProperty().Handle())
      .try_as<winrt::Microsoft::ReactNative::LayoutService>();
}

/*static*/ ReactPropertyId<LayoutService> LayoutService::LayoutServiceProperty() noexcept {
  static ReactPropertyId<LayoutService> layoutServiceProperty{L"ReactNative.UIManager", L"LayoutService"};
  return layoutServiceProperty;
}

void LayoutService::ApplyLayoutForAllNodes() noexcept {
  if (auto uiManager = ::Microsoft::ReactNative::GetNativeUIManager(*m_context).lock()) {
    uiManager->DoLayout();
  }
}

void LayoutService::ApplyLayout(int64_t reactTag, float width, float height) noexcept {
  if (auto uiManager = ::Microsoft::ReactNative::GetNativeUIManager(*m_context).lock()) {
    uiManager->ApplyLayout(reactTag, width, height);
  }
}

bool LayoutService::IsInBatch() noexcept {
  if (auto uiManager = ::Microsoft::ReactNative::GetNativeUIManager(*m_context).lock()) {
    return uiManager->isInBatch();
  }

  return false;
}

void LayoutService::MarkDirty(int64_t reactTag) noexcept {
  if (auto uiManager = ::Microsoft::ReactNative::GetNativeUIManager(*m_context).lock()) {
    uiManager->DirtyYogaNode(reactTag);
  }
}

} // namespace winrt::Microsoft::ReactNative::implementation
