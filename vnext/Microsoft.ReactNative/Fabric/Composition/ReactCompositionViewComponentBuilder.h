#pragma once
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <react/renderer/core/ReactPrimitives.h>
#include "winrt/Microsoft.ReactNative.Composition.h"
#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative::Composition {

struct ReactCompositionViewComponentBuilder : winrt::implements<
                                                  ReactCompositionViewComponentBuilder,
                                                  IReactViewComponentBuilder,
                                                  Composition::IReactCompositionViewComponentBuilder> {
  ReactCompositionViewComponentBuilder() noexcept;

 public: // IReactViewComponentBuilder
  void SetCreateProps(ViewPropsFactory impl) noexcept;

  // (Object handle, Microsoft.ReactNative.IComponentState state) => void
  void SetStateUpdater(StateUpdater impl) noexcept;

  void SetCreateShadowNode(ViewShadowNodeFactory impl) noexcept;
  void SetShadowNodeCloner(ViewShadowNodeCloner impl) noexcept;
  void SetInitialStateDataFactory(InitialStateDataFactory impl) noexcept;
  void SetMeasureContentHandler(MeasureContentHandler impl) noexcept;
  void SetLayoutHandler(LayoutHandler impl) noexcept;

 public: // Composition::IReactCompositionViewComponentBuilder
  // (ICompositionContext) => Handle
  void SetCreateView(CompositionComponentFactory impl) noexcept;
  // (Object handle, String commandName, Microsoft.ReactNative.IJSValueReader args) => bool
  void SetCommandHandler(CommandHandler commandHandler) noexcept;
  // (Object handle, Microsoft.ReactNative.IComponentProps props) => void
  void SetPropsUpdater(PropsUpdater impl) noexcept;
  // (Object handle, LayoutMetrics metrics) => void
  void SetLayoutMetricsUpdater(LayoutMetricsUpdater impl) noexcept;
  // (Object handle) => void
  void SetUpdateFinalizer(UpdateFinalizer impl) noexcept;
  // (Object handle) => IVisual
  void SetVisualCreator(VisualCreator impl) noexcept;

  void SetKeyDownHandler(KeyHandler impl) noexcept;
  void SetKeyUpHandler(KeyHandler impl) noexcept;
  void SetCharacterReceivedHandler(CharacterReceivedHandler impl) noexcept;

  void SetPointerEnteredHandler(PointerHandler impl) noexcept;
  void SetPointerExitedHandler(PointerHandler impl) noexcept;
  void SetPointerReleasedHandler(PointerHandler impl) noexcept;
  void SetPointerPressedHandler(PointerHandler impl) noexcept;
  void SetPointerMovedHandler(PointerHandler impl) noexcept;
  void SetPointerWheelChangedHandler(PointerHandler impl) noexcept;

 public:
  IComponentProps CreateProps(ViewProps props) noexcept;
  void CreateShadowNode(ShadowNode shadowNode) noexcept;
  void CloneShadowNode(ShadowNode shadowNode, ShadowNode sourceShadowNode) noexcept;
  winrt::Windows::Foundation::IInspectable InitialStateData(
      winrt::Microsoft::ReactNative::IComponentProps props) noexcept;
  MeasureContentHandler MeasureContentHandler() const noexcept;
  LayoutHandler LayoutHandler() const noexcept;

  winrt::Windows::Foundation::IInspectable CreateView(IReactContext reactContext, ICompositionContext context) noexcept;
  bool HandleCommand(
      winrt::Windows::Foundation::IInspectable handle,
      winrt::hstring commandName,
      IJSValueReader args) noexcept;
  void UpdateProps(winrt::Windows::Foundation::IInspectable handle, IComponentProps props) noexcept;
  void UpdateState(winrt::Windows::Foundation::IInspectable handle, IComponentState state) noexcept;
  void UpdateLayoutMetrics(winrt::Windows::Foundation::IInspectable handle, LayoutMetrics metrics) noexcept;
  void FinalizeUpdates(winrt::Windows::Foundation::IInspectable handle) noexcept;
  IVisual CreateVisual(winrt::Windows::Foundation::IInspectable handle) noexcept;
  void OnKeyDown(
      winrt::Windows::Foundation::IInspectable handle,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept;
  void OnKeyUp(
      winrt::Windows::Foundation::IInspectable handle,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept;
  void OnCharacterReceived(
      winrt::Windows::Foundation::IInspectable handle,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::CharacterReceivedRoutedEventArgs &args) noexcept;
  void OnPointerEntered(
      winrt::Windows::Foundation::IInspectable handle,
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept;
  void OnPointerExited(
      winrt::Windows::Foundation::IInspectable handle,
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept;
  void OnPointerPressed(
      winrt::Windows::Foundation::IInspectable handle,
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept;
  void OnPointerReleased(
      winrt::Windows::Foundation::IInspectable handle,
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept;
  void OnPointerMoved(
      winrt::Windows::Foundation::IInspectable handle,
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept;
  void OnPointerWheelChanged(
      winrt::Windows::Foundation::IInspectable handle,
      const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept;

 private:
  ViewPropsFactory m_propsFactory;
  StateUpdater m_stateUpdater;
  ViewShadowNodeFactory m_shadowNodeFactory;
  ViewShadowNodeCloner m_shadowNodeCloner;
  InitialStateDataFactory m_initialStateDataFactory;
  winrt::Microsoft::ReactNative::MeasureContentHandler m_measureContent;
  winrt::Microsoft::ReactNative::LayoutHandler m_layoutHandler;

  CompositionComponentFactory m_createView;
  CommandHandler m_commandHandler;
  PropsUpdater m_propsUpdater;
  LayoutMetricsUpdater m_layoutMetricsUpdater;
  UpdateFinalizer m_finalizer;
  VisualCreator m_visualCreator;
  KeyHandler m_keyUp;
  KeyHandler m_keyDown;
  CharacterReceivedHandler m_characterReceived;
  PointerHandler m_pointerEntered;
  PointerHandler m_pointerExited;
  PointerHandler m_pointerReleased;
  PointerHandler m_pointerPressed;
  PointerHandler m_pointerMoved;
  PointerHandler m_pointerWheelChanged;
};

} // namespace winrt::Microsoft::ReactNative::Composition
