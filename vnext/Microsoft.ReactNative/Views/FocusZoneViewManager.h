// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Views/ShadowNodeBase.h>
#include <Views/ViewPanel.h>
#include <winrt/Windows.UI.Xaml.h>
#include "Views/ViewViewManager.h"

namespace winrt {
using ContentControl = winrt::Windows::UI::Xaml::Controls::ContentControl;
using namespace winrt::Windows::UI::Xaml;
} // namespace winrt

namespace Microsoft::ReactNative {

class FocusZoneViewShadowNode : public ViewShadowNode {
  using Super = ViewShadowNode;

 public:
  FocusZoneViewShadowNode() = default;

  void createView() override;
  void updateProperties(winrt::Microsoft::ReactNative::JSValueObject &props);

 private:
  void OnKeyDown(const winrt::IInspectable &sender, const xaml::Input::KeyRoutedEventArgs &e);
  void OnGettingFocus(const winrt::IInspectable &sender, const xaml::Input::GettingFocusEventArgs &e);
  void OnLosingFocus(const winrt::IInspectable &sender, const xaml::Input::LosingFocusEventArgs &e);

  bool m_isHorizontal = false;
  winrt::weak_ref<winrt::DependencyObject> m_lastElement = nullptr;
  xaml::UIElement::PreviewKeyDown_revoker m_panelKeyDownRevoker{};
  xaml::UIElement::GettingFocus_revoker m_panelGettingFocusRevoker{};
  xaml::UIElement::LosingFocus_revoker m_panelLosingFocusRevoker{};
};

class FocusZoneViewManager : public ViewViewManager {
  using Super = ViewViewManager;

 public:
  FocusZoneViewManager(const Mso::React::IReactContext &context);

  const wchar_t *GetName() const override;

  void GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;
  ShadowNode *createShadow() const override;
};

} // namespace Microsoft::ReactNative

// @generated SignedSource<<9b821f0216bf3cad2629d3d373cb14bd>>