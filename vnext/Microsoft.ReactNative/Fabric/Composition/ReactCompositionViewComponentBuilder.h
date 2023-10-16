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
  // (Object handle, UInt32 Msg, UInt64 WParam, Int64 LParam) => Int64
  void SetMessageHandler(MessageHandler impl) noexcept;

  void SetKeyDownHandler(KeyHandler impl) noexcept;
  void SetKeyUpHandler(KeyHandler impl) noexcept;

  void SetPointerEnteredHandler(PointerHandler impl) noexcept;
  void SetPointerExitedHandler(PointerHandler impl) noexcept;
  void SetPointerReleasedHandler(PointerHandler impl) noexcept;
  void SetPointerPressedHandler(PointerHandler impl) noexcept;
  void SetPointerMovedHandler(PointerHandler impl) noexcept;
  void SetPointerWheelChangedHandler(PointerHandler impl) noexcept;

 public:
  IComponentProps CreateProps(ViewProps props) noexcept;

  winrt::Windows::Foundation::IInspectable CreateView(IReactContext reactContext, ICompositionContext context) noexcept;
  bool HandleCommand(
      winrt::Windows::Foundation::IInspectable handle,
      winrt::hstring commandName,
      IJSValueReader args) noexcept;
  void UpdateProps(winrt::Windows::Foundation::IInspectable handle, IComponentProps props) noexcept;
  void UpdateLayoutMetrics(winrt::Windows::Foundation::IInspectable handle, LayoutMetrics metrics) noexcept;
  void FinalizeUpdates(winrt::Windows::Foundation::IInspectable handle) noexcept;
  IVisual CreateVisual(winrt::Windows::Foundation::IInspectable handle) noexcept;
  int64_t
  SendMessage(winrt::Windows::Foundation::IInspectable handle, uint32_t Msg, uint64_t WParam, int64_t LParam) noexcept;
  void OnKeyDown(
      winrt::Windows::Foundation::IInspectable handle,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept;
  void OnKeyUp(
      winrt::Windows::Foundation::IInspectable handle,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept;
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

  CompositionComponentFactory m_createView;
  CommandHandler m_commandHandler;
  PropsUpdater m_propsUpdater;
  LayoutMetricsUpdater m_layoutMetricsUpdater;
  UpdateFinalizer m_finalizer;
  VisualCreator m_visualCreator;
  MessageHandler m_messageHandler;
  KeyHandler m_keyUp;
  KeyHandler m_keyDown;
  PointerHandler m_pointerEntered;
  PointerHandler m_pointerExited;
  PointerHandler m_pointerReleased;
  PointerHandler m_pointerPressed;
  PointerHandler m_pointerMoved;
  PointerHandler m_pointerWheelChanged;
};

} // namespace winrt::Microsoft::ReactNative::Composition
