// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactCompositionViewComponentBuilder.h"
#include <strsafe.h>
#include "DynamicWriter.h"
#include "ReactHost/MsoUtils.h"

namespace winrt::Microsoft::ReactNative::Composition {

//===========================================================================
// ReactCompositionViewComponentBuilder implementation
//===========================================================================

ReactCompositionViewComponentBuilder::ReactCompositionViewComponentBuilder() noexcept {}

void ReactCompositionViewComponentBuilder::SetCreateProps(ViewPropsFactory impl) noexcept {
  m_propsFactory = impl;
}

IComponentProps ReactCompositionViewComponentBuilder::CreateProps(ViewProps props) noexcept {
  return m_propsFactory(props);
}

void ReactCompositionViewComponentBuilder::SetCreateView(CompositionComponentFactory impl) noexcept {
  m_createView = impl;
}
// (Object handle, String commandName, Microsoft.ReactNative.IJSValueReader args) => bool
void ReactCompositionViewComponentBuilder::SetCommandHandler(Composition::CommandHandler commandHandler) noexcept {
  m_commandHandler = commandHandler;
}
// (Object handle, Microsoft.ReactNative.IComponentProps props) => void
void ReactCompositionViewComponentBuilder::SetPropsUpdater(PropsUpdater impl) noexcept {
  m_propsUpdater = impl;
}
// (Object handle, LayoutMetrics metrics) => void
void ReactCompositionViewComponentBuilder::SetLayoutMetricsUpdater(LayoutMetricsUpdater impl) noexcept {
  m_layoutMetricsUpdater = impl;
}
// (Object handle) => void
void ReactCompositionViewComponentBuilder::SetUpdateFinalizer(UpdateFinalizer impl) noexcept {
  m_finalizer = impl;
}
// (Object handle) => IVisual
void ReactCompositionViewComponentBuilder::SetVisualCreator(Composition::VisualCreator impl) noexcept {
  m_visualCreator = impl;
}
// (Object handle, UInt32 Msg, UInt64 WParam, Int64 LParam) => Int64
void ReactCompositionViewComponentBuilder::SetMessageHandler(MessageHandler impl) noexcept {
  m_messageHandler = impl;
}

void ReactCompositionViewComponentBuilder::SetKeyDownHandler(KeyHandler impl) noexcept {
  m_keyDown = impl;
}

void ReactCompositionViewComponentBuilder::SetKeyUpHandler(KeyHandler impl) noexcept {
  m_keyUp = impl;
}

winrt::Windows::Foundation::IInspectable ReactCompositionViewComponentBuilder::CreateView(
    IReactContext reactContext,
    ICompositionContext context) noexcept {
  return m_createView(reactContext, context);
}

bool ReactCompositionViewComponentBuilder::HandleCommand(
    winrt::Windows::Foundation::IInspectable handle,
    winrt::hstring commandName,
    IJSValueReader args) noexcept {
  if (!m_commandHandler) {
    return false;
  }
  return m_commandHandler(handle, commandName, args);
}

void ReactCompositionViewComponentBuilder::UpdateProps(
    winrt::Windows::Foundation::IInspectable handle,
    IComponentProps props) noexcept {
  if (m_propsUpdater) {
    m_propsUpdater(handle, props);
  }
}

void ReactCompositionViewComponentBuilder::UpdateLayoutMetrics(
    winrt::Windows::Foundation::IInspectable handle,
    Composition::LayoutMetrics metrics) noexcept {
  if (m_layoutMetricsUpdater) {
    m_layoutMetricsUpdater(handle, metrics);
  }
}

void ReactCompositionViewComponentBuilder::FinalizeUpdates(winrt::Windows::Foundation::IInspectable handle) noexcept {
  if (m_finalizer) {
    m_finalizer(handle);
  }
}

IVisual ReactCompositionViewComponentBuilder::CreateVisual(winrt::Windows::Foundation::IInspectable handle) noexcept {
  return m_visualCreator(handle);
}

int64_t ReactCompositionViewComponentBuilder::SendMessage(
    winrt::Windows::Foundation::IInspectable handle,
    uint32_t msg,
    uint64_t wparam,
    int64_t lparam) noexcept {
  if (!m_messageHandler) {
    return 0;
  }
  return m_messageHandler(handle, msg, wparam, lparam);
}

void ReactCompositionViewComponentBuilder::OnKeyDown(
    const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
    const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept {
  if (m_keyDown) {
    m_keyDown(source, args);
  }
}

void ReactCompositionViewComponentBuilder::OnKeyUp(
    const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
    const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept {
  if (m_keyUp) {
    m_keyUp(source, args);
  }
}

} // namespace winrt::Microsoft::ReactNative::Composition
