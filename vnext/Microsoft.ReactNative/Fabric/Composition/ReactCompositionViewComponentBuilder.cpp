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

winrt::IInspectable ReactCompositionViewComponentBuilder::CreateView(ICompositionContext context) noexcept
{
  return m_createView(context);
}

bool ReactCompositionViewComponentBuilder::HandeCommand(
  winrt::IInspectable handle,
  winrt::hstring commandName,
  IJSValueReader args) noexcept
{
  if (!m_commandHandler) {
    return false;
  }
  return m_commandHandler(handle, commandName, args);
}

void ReactCompositionViewComponentBuilder::UpdateProps(winrt::IInspectable handle, IComponentProps props) noexcept
{
  m_propsUpdater(handle, props);
}

void ReactCompositionViewComponentBuilder::UpdateLayoutMetrics(
  winrt::IInspectable handle,
  Composition::LayoutMetrics metrics) noexcept
{
  m_layoutMetricsUpdater(handle, metrics);
}

void ReactCompositionViewComponentBuilder::FinalizeUpdates(winrt::IInspectable handle) noexcept {
  if (m_finalizer) {
    m_finalizer(handle);
  }
}

IVisual ReactCompositionViewComponentBuilder::CreateVisual(winrt::IInspectable handle) noexcept {
  return m_visualCreator(handle);
}

int64_t ReactCompositionViewComponentBuilder::SendMessage(
  winrt::IInspectable handle,
    uint32_t msg,
    uint64_t wparam,
    int64_t lparam) noexcept {
  if (!m_messageHandler) {
    return 0;
  }
  return m_messageHandler(handle, msg, wparam, lparam);
}

} // namespace winrt::Microsoft::ReactNative::Composition
