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

 public:
  IComponentProps CreateProps(ViewProps props) noexcept;

  winrt::Windows::Foundation::IInspectable CreateView(ICompositionContext context) noexcept;
  bool HandeCommand(
      winrt::Windows::Foundation::IInspectable handle,
      winrt::hstring commandName,
      IJSValueReader args) noexcept;
  void UpdateProps(winrt::Windows::Foundation::IInspectable handle, IComponentProps props) noexcept;
  void UpdateLayoutMetrics(winrt::Windows::Foundation::IInspectable handle, LayoutMetrics metrics) noexcept;
  void FinalizeUpdates(winrt::Windows::Foundation::IInspectable handle) noexcept;
  IVisual CreateVisual(winrt::Windows::Foundation::IInspectable handle) noexcept;
  int64_t
  SendMessage(winrt::Windows::Foundation::IInspectable handle, uint32_t Msg, uint64_t WParam, int64_t LParam) noexcept;

 private:
  ViewPropsFactory m_propsFactory;

  CompositionComponentFactory m_createView;
  CommandHandler m_commandHandler;
  PropsUpdater m_propsUpdater;
  LayoutMetricsUpdater m_layoutMetricsUpdater;
  UpdateFinalizer m_finalizer;
  VisualCreator m_visualCreator;
  MessageHandler m_messageHandler;
};

} // namespace winrt::Microsoft::ReactNative::Composition
