
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/ComponentView.h>
#include <Microsoft.ReactNative.Cxx/ReactContext.h>
#include <react/renderer/components/view/ViewEventEmitter.h>
#include <react/renderer/components/view/ViewProps.h>
#include "CompositionHelpers.h"

#include "Composition.ComponentView.g.h"
#include "Composition.CreateCompositionComponentViewArgs.g.h"
#include "Composition.ViewComponentView.g.h"

namespace Microsoft::ReactNative {
struct CompContext;
} // namespace Microsoft::ReactNative

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct CreateCompositionComponentViewArgs
    : public CreateCompositionComponentViewArgsT<
          CreateCompositionComponentViewArgs,
          winrt::Microsoft::ReactNative::implementation::CreateComponentViewArgs,
          winrt::Microsoft::ReactNative::Composition::Experimental::IInternalCreateComponentViewArgs> {
  CreateCompositionComponentViewArgs(
      const winrt::Microsoft::ReactNative::IReactContext &reactContext,
      facebook::react::Tag tag,
      const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compositionContext);

  winrt::Microsoft::UI::Composition::Compositor Compositor() const noexcept;

  winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext CompositionContext() const noexcept;

  ComponentViewFeatures Features() const noexcept;
  void Features(ComponentViewFeatures value) noexcept;

 private:
  ComponentViewFeatures m_features{ComponentViewFeatures::Default};
  winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext m_compositionContext;
};

struct ComponentView
    : public ComponentViewT<ComponentView, winrt::Microsoft::ReactNative::implementation::ComponentView> {
  static constexpr size_t SpecialBorderLayerCount = 8;

  ComponentView(winrt::Microsoft::ReactNative::Composition::CreateCompositionComponentViewArgs const &args);
  ComponentView(
      const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext,
      ComponentViewFeatures flags,
      bool customControl);

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
  void onLostFocus(const winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs& args) noexcept override;
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
  virtual winrt::Microsoft::ReactNative::implementation::ClipState getClipState() noexcept;

  virtual std::string DefaultControlType() const noexcept;
  virtual std::string DefaultAccessibleName() const noexcept;
  virtual std::string DefaultHelpText() const noexcept;

  winrt::Microsoft::UI::Composition::Compositor Compositor() const noexcept;
  winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext CompositionContext() const noexcept;

  // Publicaly overridable APIs
  void FinalizeUpdates(winrt::Microsoft::ReactNative::ComponentViewUpdateMask updateMask) noexcept override;
  virtual void OnThemeChanged() noexcept;

 protected:
  bool anyHitTestHelper(
      facebook::react::Tag &targetTag,
      facebook::react::Point &ptContent,
      facebook::react::Point &localPt) const noexcept;

  winrt::IInspectable m_uiaProvider{nullptr};
  winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext m_compContext;
  comp::CompositionPropertySet m_centerPropSet{nullptr};
  facebook::react::SharedViewEventEmitter m_eventEmitter;
  facebook::react::LayoutMetrics m_layoutMetrics;
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

  ComponentViewFeatures m_flags;
  void showFocusVisual(bool show) noexcept;
  winrt::Microsoft::ReactNative::Composition::Experimental::IFocusVisual m_focusVisual{nullptr};
  winrt::Microsoft::ReactNative::Composition::Experimental::IVisual m_outerVisual{nullptr};
};

struct ViewComponentView : public ViewComponentViewT<ViewComponentView, ComponentView> {
  ViewComponentView(winrt::Microsoft::ReactNative::Composition::CreateCompositionComponentViewArgs const &args);
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
  void OnKeyDown(
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept override;
  void OnKeyUp(
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept override;
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
      ComponentViewFeatures flags,
      bool customComponent);

  virtual winrt::Microsoft::ReactNative::Composition::Experimental::IVisual createVisual() noexcept;

  // Publicly overridable APIs
  virtual winrt::Microsoft::UI::Composition::Visual CreateVisual() noexcept;
  virtual void UpdateLayoutMetrics(const LayoutMetrics &metrics, const LayoutMetrics &oldMetrics) noexcept;

 protected:
  virtual winrt::Microsoft::ReactNative::ViewProps ViewPropsInner() noexcept;

 private:
  facebook::react::SharedViewProps m_props;
  winrt::Microsoft::ReactNative::Composition::Experimental::IVisual m_visual{nullptr};
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation

namespace winrt::Microsoft::ReactNative::Composition::factory_implementation {

struct ComponentView : ComponentViewT<ComponentView, implementation::ComponentView> {};

struct ViewComponentView : ViewComponentViewT<ViewComponentView, implementation::ViewComponentView> {};

} // namespace winrt::Microsoft::ReactNative::Composition::factory_implementation
