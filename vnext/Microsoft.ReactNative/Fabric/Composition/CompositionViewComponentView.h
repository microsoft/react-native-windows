
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

enum class CompositionComponentViewFeatures : std::uint_fast8_t {
  None = 0,
  NativeBorder = 1 << 0, // Standard border handling
  ShadowProps = 1 << 1, // Apply shadow to visual

  Default = NativeBorder
};

DEFINE_ENUM_FLAG_OPERATORS(CompositionComponentViewFeatures);

struct CompositionBaseComponentView : public ComponentViewT<
                                          CompositionBaseComponentView,
                                          winrt::Microsoft::ReactNative::implementation::ComponentView> {
  static constexpr size_t SpecialBorderLayerCount = 8;

  CompositionBaseComponentView(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext,
      CompositionComponentViewFeatures flags);

  virtual winrt::Microsoft::ReactNative::Composition::IVisual Visual() const noexcept = 0;
  // Visual that should be parented to this ComponentView's parent
  virtual winrt::Microsoft::ReactNative::Composition::IVisual OuterVisual() const noexcept;
  void updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept override;
  const facebook::react::SharedViewEventEmitter &GetEventEmitter() const noexcept;
  void handleCommand(std::string const &commandName, folly::dynamic const &arg) noexcept override;
  RootComponentView *rootComponentView() noexcept override;
  void parent(const winrt::Microsoft::ReactNative::ComponentView &parent) noexcept override;
  winrt::Microsoft::ReactNative::implementation::ComponentView *parent() const noexcept override;
  winrt::Microsoft::ReactNative::ComponentView Parent() const noexcept override;
  winrt::IVectorView<winrt::Microsoft::ReactNative::ComponentView> Children() const noexcept override;
  facebook::react::Props::Shared props() noexcept override;
  virtual facebook::react::SharedViewProps viewProps() noexcept = 0;
  void theme(winrt::Microsoft::ReactNative::Composition::implementation::Theme *theme) noexcept override;
  winrt::Microsoft::ReactNative::Composition::implementation::Theme *theme() const noexcept override;
  void Theme(const winrt::Microsoft::ReactNative::Composition::Theme &theme) noexcept;
  winrt::Microsoft::ReactNative::Composition::Theme Theme() const noexcept;
  void onThemeChanged() noexcept override;
  bool runOnChildren(
      bool forward,
      Mso::Functor<bool(winrt::Microsoft::ReactNative::implementation::ComponentView &)> &fn) noexcept override;
  void onFocusLost() noexcept override;
  void onFocusGained() noexcept override;
  void onPointerEntered(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;
  void onPointerExited(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;
  void onPointerPressed(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;
  void onPointerReleased(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;
  void onPointerMoved(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;
  void onPointerWheelChanged(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept override;

  void onKeyDown(
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept override;
  void onKeyUp(
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept override;
  void onCharacterReceived(
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::CharacterReceivedRoutedEventArgs &args) noexcept
      override;

  bool focusable() const noexcept override;
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
  void finalizeUpdates(
      winrt::Microsoft::ReactNative::implementation::RNComponentViewUpdateMask updateMask) noexcept override;

  void indexOffsetForBorder(uint32_t &index) const noexcept;
  void updateShadowProps(
      const facebook::react::ViewProps &oldViewProps,
      const facebook::react::ViewProps &newViewProps) noexcept;
  void applyShadowProps(const facebook::react::ViewProps &viewProps) noexcept;
  void updateTransformProps(
      const facebook::react::ViewProps &oldViewProps,
      const facebook::react::ViewProps &newViewProps,
      winrt::Microsoft::ReactNative::Composition::ISpriteVisual m_visual) noexcept;
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
  winrt::Microsoft::ReactNative::implementation::ClipState getClipState() noexcept override;

  virtual std::string DefaultControlType() const noexcept;
  virtual std::string DefaultAccessibleName() const noexcept;
  virtual std::string DefaultHelpText() const noexcept;

 protected:
  bool anyHitTestHelper(facebook::react::Tag& targetTag, facebook::react::Point& ptContent, facebook::react::Point& localPt) const noexcept;

  winrt::IInspectable m_uiaProvider{nullptr};
  winrt::Microsoft::ReactNative::Composition::ICompositionContext m_compContext;
  comp::CompositionPropertySet m_centerPropSet{nullptr};
  const facebook::react::Tag m_tag;
  facebook::react::SharedViewEventEmitter m_eventEmitter;
  winrt::Windows::Foundation::Collections::IVector<winrt::Microsoft::ReactNative::ComponentView> m_children{ winrt::single_threaded_vector<winrt::Microsoft::ReactNative::ComponentView>() };
  winrt::Microsoft::ReactNative::ComponentView m_parent{nullptr};
  RootComponentView *m_rootView{nullptr};
  facebook::react::LayoutMetrics m_layoutMetrics;
  bool m_needsBorderUpdate{false};
  bool m_hasTransformMatrixFacade{false};
  bool m_enableFocusVisual{false};
  uint8_t m_numBorderVisuals{0};
  winrt::Microsoft::ReactNative::ReactContext m_context;

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
      std::array<winrt::Microsoft::ReactNative::Composition::ISpriteVisual, SpecialBorderLayerCount> &spBorderVisuals,
      facebook::react::LayoutMetrics const &layoutMetrics,
      const facebook::react::ViewProps &viewProps) noexcept;
  std::array<winrt::Microsoft::ReactNative::Composition::ISpriteVisual, SpecialBorderLayerCount>
  FindSpecialBorderLayers() const noexcept;
  void UpdateCenterPropertySet() noexcept;

  CompositionComponentViewFeatures m_flags;
  mutable winrt::Microsoft::ReactNative::Composition::implementation::Theme *m_theme{nullptr};
  void showFocusVisual(bool show) noexcept;
  winrt::Microsoft::ReactNative::Composition::IFocusVisual m_focusVisual{nullptr};
  winrt::Microsoft::ReactNative::Composition::IVisual m_outerVisual{nullptr};
};

struct CompositionViewComponentView
    : public ViewComponentViewT<CompositionViewComponentView, CompositionBaseComponentView> {
  using Super = ViewComponentViewT<ViewComponentView, CompositionViewComponentView>;

  [[nodiscard]] static winrt::Microsoft::ReactNative::ComponentView Create(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  void mountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
      uint32_t index) noexcept override;
  void unmountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
      uint32_t index) noexcept override;
  void updateProps(facebook::react::Props::Shared const &props, facebook::react::Props::Shared const &oldProps) noexcept
      override;
  void updateState(facebook::react::State::Shared const &state, facebook::react::State::Shared const &oldState) noexcept
      override;
  void updateLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept override;
  void prepareForRecycle() noexcept override;
  bool focusable() const noexcept override;
  void onKeyDown(
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept override;
  void onKeyUp(
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept override;
  std::string DefaultControlType() const noexcept override;

  facebook::react::SharedViewProps viewProps() noexcept override;
  winrt::Microsoft::ReactNative::ViewProps ViewProps() noexcept;

  void onThemeChanged() noexcept override;

  facebook::react::Tag hitTest(
      facebook::react::Point pt,
      facebook::react::Point &localPt,
      bool ignorePointerEvents = false) const noexcept override;

  winrt::Microsoft::ReactNative::Composition::IVisual Visual() const noexcept override;

  CompositionViewComponentView(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext);

 private:
  facebook::react::SharedViewProps m_props;
  winrt::Microsoft::ReactNative::Composition::ISpriteVisual m_visual{nullptr};
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
