
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/ComponentView.h>
#include <Microsoft.ReactNative.Cxx/ReactContext.h>
#include <react/renderer/components/view/ViewEventEmitter.h>
#include <react/renderer/components/view/ViewProps.h>
#include "CompositionHelpers.h"

#include "Composition.ComponentView.g.h"
#include "Composition.ViewComponentView.g.h"

namespace Microsoft::ReactNative {
struct CompContext;
} // namespace Microsoft::ReactNative

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct ComponentView : public ComponentViewT<
                           ComponentView,
                           winrt::Microsoft::ReactNative::implementation::ComponentView,
                           winrt::Microsoft::ReactNative::Composition::Experimental::IInternalComponentView> {
  static constexpr size_t SpecialBorderLayerCount = 8;

  ComponentView(
      const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext,
      ComponentViewFeatures flags);

  virtual winrt::Microsoft::ReactNative::Composition::Experimental::IVisual Visual() const noexcept {
    return nullptr;
  };
  // Visual that should be parented to this ComponentView's parent
  virtual winrt::Microsoft::ReactNative::Composition::Experimental::IVisual OuterVisual() const noexcept;
  void updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept override;
  const facebook::react::SharedViewEventEmitter &GetEventEmitter() const noexcept;
  void HandleCommand(winrt::hstring commandName, const winrt::Microsoft::ReactNative::IJSValueReader &args) noexcept
      override;
  facebook::react::Props::Shared props() noexcept override;
  virtual const facebook::react::SharedViewProps &viewProps() const noexcept {
    static facebook::react::SharedViewProps emptyProps;
    assert(false);
    return emptyProps;
  };

  winrt::Microsoft::ReactNative::Composition::RootComponentView Root() noexcept;

  void Theme(const winrt::Microsoft::ReactNative::Composition::Theme &theme) noexcept;
  winrt::Microsoft::ReactNative::Composition::Theme Theme() const noexcept;
  void onThemeChanged() noexcept override;
  void onLostFocus(const winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs &args) noexcept override;
  void onGotFocus(const winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs &args) noexcept override;
  bool CapturePointer(const winrt::Microsoft::ReactNative::Composition::Input::Pointer &pointer) noexcept;
  void ReleasePointerCapture(const winrt::Microsoft::ReactNative::Composition::Input::Pointer &pointer) noexcept;

  std::vector<facebook::react::ComponentDescriptorProvider> supplementalComponentDescriptorProviders() noexcept
      override;
  facebook::react::SharedViewEventEmitter eventEmitter() noexcept override;
  facebook::react::SharedViewEventEmitter eventEmitterAtPoint(facebook::react::Point pt) noexcept override;
  facebook::react::Tag Tag() const noexcept override;

  RECT getClientRect() const noexcept override;
  facebook::react::Point getClientOffset() const noexcept override;
  void updateProps(facebook::react::Props::Shared const &props, facebook::react::Props::Shared const &oldProps) noexcept
      override;
  void updateLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept override;

  void indexOffsetForBorder(uint32_t &index) const noexcept;
  void updateShadowProps(
      const facebook::react::ViewProps &oldViewProps,
      const facebook::react::ViewProps &newViewProps) noexcept;
  void applyShadowProps(const facebook::react::ViewProps &viewProps) noexcept;
  void updateTransformProps(
      const facebook::react::ViewProps &oldViewProps,
      const facebook::react::ViewProps &newViewProps,
      winrt::Microsoft::ReactNative::Composition::Experimental::IVisual m_visual) noexcept;
  void updateAccessibilityProps(
      const facebook::react::ViewProps &oldView,
      const facebook::react::ViewProps &newViewProps) noexcept;

  virtual void OnRenderingDeviceLost() noexcept;

  void StartBringIntoView(winrt::Microsoft::ReactNative::implementation::BringIntoViewOptions &&args) noexcept override;

  comp::CompositionPropertySet EnsureCenterPointPropertySet() noexcept;
  void EnsureTransformMatrixFacade() noexcept;

  winrt::IInspectable EnsureUiaProvider() noexcept override;
  std::optional<std::string> getAcccessiblityValue() noexcept override;
  void setAcccessiblityValue(std::string &&value) noexcept override;
  bool getAcccessiblityIsReadOnly() noexcept override;
  ToggleState getToggleState() noexcept override;
  void Toggle() noexcept override;
  virtual winrt::Microsoft::ReactNative::implementation::ClipState getClipState() noexcept;

  const facebook::react::LayoutMetrics &layoutMetrics() const noexcept;

  virtual std::string DefaultControlType() const noexcept;
  virtual std::string DefaultAccessibleName() const noexcept;
  virtual std::string DefaultHelpText() const noexcept;

  winrt::Microsoft::UI::Composition::Compositor Compositor() const noexcept;
  winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext CompositionContext() const noexcept;

  void FinalizeUpdates(winrt::Microsoft::ReactNative::ComponentViewUpdateMask updateMask) noexcept override;
  winrt::event_token ThemeChanged(
      winrt::Windows::Foundation::EventHandler<winrt::IInspectable> const &handler) noexcept;
  void ThemeChanged(winrt::event_token const &token) noexcept;

 protected:
  bool anyHitTestHelper(
      facebook::react::Tag &targetTag,
      facebook::react::Point &ptContent,
      facebook::react::Point &localPt) const noexcept;

  winrt::IInspectable m_uiaProvider{nullptr};
  winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext m_compContext;
  comp::CompositionPropertySet m_centerPropSet{nullptr};
  facebook::react::SharedViewEventEmitter m_eventEmitter;
  bool m_needsBorderUpdate{false};
  bool m_hasTransformMatrixFacade{false};
  bool m_enableFocusVisual{false};
  uint8_t m_numBorderVisuals{0};

 private:
  void updateBorderProps(
      const facebook::react::ViewProps &oldViewProps,
      const facebook::react::ViewProps &newViewProps) noexcept;
  void updateBorderLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      const facebook::react::ViewProps &viewProps) noexcept;
  void finalizeBorderUpdates(
      facebook::react::LayoutMetrics const &layoutMetrics,
      const facebook::react::ViewProps &viewProps) noexcept;
  bool TryUpdateSpecialBorderLayers(
      winrt::Microsoft::ReactNative::Composition::implementation::Theme *theme,
      std::array<winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual, SpecialBorderLayerCount>
          &spBorderVisuals,
      facebook::react::LayoutMetrics const &layoutMetrics,
      const facebook::react::ViewProps &viewProps) noexcept;
  std::array<winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual, SpecialBorderLayerCount>
  FindSpecialBorderLayers() const noexcept;
  void UpdateCenterPropertySet() noexcept;
  void FinalizeTransform(
      facebook::react::LayoutMetrics const &layoutMetrics,
      const facebook::react::ViewProps &viewProps) noexcept;

  bool m_FinalizeTransform{false};
  ComponentViewFeatures m_flags;
  void showFocusVisual(bool show) noexcept;
  winrt::Microsoft::ReactNative::Composition::Experimental::IFocusVisual m_focusVisual{nullptr};
  winrt::Microsoft::ReactNative::Composition::Experimental::IVisual m_outerVisual{nullptr};
  winrt::event<winrt::Windows::Foundation::EventHandler<winrt::IInspectable>> m_themeChangedEvent;
};

struct ViewComponentView : public ViewComponentViewT<
                               ViewComponentView,
                               ComponentView,
                               winrt::Microsoft::ReactNative::Composition::Experimental::IInternalCreateVisual> {
  [[nodiscard]] static winrt::Microsoft::ReactNative::ComponentView Create(
      const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  void MountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
      uint32_t index) noexcept override;
  void UnmountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
      uint32_t index) noexcept override;
  void updateProps(facebook::react::Props::Shared const &props, facebook::react::Props::Shared const &oldProps) noexcept
      override;
  void updateLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept override;
  void prepareForRecycle() noexcept override;
  bool TryFocus() noexcept;
  bool focusable() const noexcept override;
  void OnKeyDown(const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept override;
  void OnKeyUp(const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept override;
  std::string DefaultControlType() const noexcept override;

  const facebook::react::SharedViewProps &viewProps() const noexcept override;
  winrt::Microsoft::ReactNative::ViewProps ViewProps() noexcept;

  facebook::react::Tag hitTest(
      facebook::react::Point pt,
      facebook::react::Point &localPt,
      bool ignorePointerEvents = false) const noexcept override;
  const winrt::Microsoft::ReactNative::IComponentProps userProps(
      facebook::react::Props::Shared const &props) noexcept override;
  winrt::Microsoft::ReactNative::Composition::Experimental::IVisual Visual() const noexcept override;
  void ensureVisual() noexcept;

  static facebook::react::SharedViewProps defaultProps() noexcept;

  ViewComponentView(
      const facebook::react::SharedViewProps &defaultProps,
      const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext,
      ComponentViewFeatures flags);

  virtual winrt::Microsoft::ReactNative::Composition::Experimental::IVisual createVisual() noexcept;

  void CreateVisualHandler(const winrt::Microsoft::ReactNative::Composition::CreateVisualDelegate &handler);
  winrt::Microsoft::ReactNative::Composition::CreateVisualDelegate CreateVisualHandler() const noexcept;
  void CreateInternalVisualHandler(
      const winrt::Microsoft::ReactNative::Composition::Experimental::CreateInternalVisualDelegate &handler);
  winrt::Microsoft::ReactNative::Composition::Experimental::CreateInternalVisualDelegate CreateInternalVisualHandler()
      const noexcept;

 protected:
  virtual winrt::Microsoft::ReactNative::ViewProps ViewPropsInner() noexcept;

 private:
  bool m_hasNonVisualChildren{false};
  facebook::react::SharedViewProps m_props;
  winrt::Microsoft::ReactNative::Composition::Experimental::IVisual m_visual{nullptr};
  winrt::Microsoft::ReactNative::Composition::CreateVisualDelegate m_createVisualHandler{nullptr};
  winrt::Microsoft::ReactNative::Composition::Experimental::CreateInternalVisualDelegate m_createInternalVisualHandler{
      nullptr};
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
