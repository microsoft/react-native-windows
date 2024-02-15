// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactCompositionViewComponentBuilder.h"
#include <Fabric/Composition/CompositionViewComponentView.h>
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

void ReactCompositionViewComponentBuilder::CreateShadowNode(ShadowNode shadowNode) noexcept {
  if (m_shadowNodeFactory) {
    return m_shadowNodeFactory(shadowNode);
  }
}

void ReactCompositionViewComponentBuilder::CloneShadowNode(
    ShadowNode shadowNode,
    ShadowNode sourceShadowNode) noexcept {
  if (m_shadowNodeCloner) {
    m_shadowNodeCloner(shadowNode, sourceShadowNode);
  }
}

MeasureContentHandler ReactCompositionViewComponentBuilder::MeasureContentHandler() const noexcept {
  return m_measureContent;
}

LayoutHandler ReactCompositionViewComponentBuilder::LayoutHandler() const noexcept {
  return m_layoutHandler;
}

void ReactCompositionViewComponentBuilder::SetCreateComponentView(ComponentViewFactory impl) noexcept {
  m_createComponentView = impl;
  assert(!m_createView); // Only SetCreateComponentView OR SetCreateViewComponentView should be called
}

void ReactCompositionViewComponentBuilder::SetCreateViewComponentView(
    CompositionViewComponentViewFactory impl) noexcept {
  m_createView = impl;
  assert(!m_createComponentView); // Only SetCreateComponentView OR SetCreateViewComponentView should be called
}
// (Object handle, Microsoft.ReactNative.IComponentState state) => void
// void ReactCompositionViewComponentBuilder::SetStateUpdater(StateUpdater impl) noexcept {
//  m_stateUpdater = impl;
// }
// () => Object
void ReactCompositionViewComponentBuilder::SetCreateShadowNode(ViewShadowNodeFactory impl) noexcept {
  m_shadowNodeFactory = impl;
}
// (Object handle) => Object
void ReactCompositionViewComponentBuilder::SetShadowNodeCloner(ViewShadowNodeCloner impl) noexcept {
  m_shadowNodeCloner = impl;
}
// (ShadowNode shadowNode, LayoutContext layoutContext, LayoutConstraints layoutConstraints) -> Size
void ReactCompositionViewComponentBuilder::SetMeasureContentHandler(
    winrt::Microsoft::ReactNative::MeasureContentHandler impl) noexcept {
  m_measureContent = impl;
}

void ReactCompositionViewComponentBuilder::SetInitialStateDataFactory(InitialStateDataFactory impl) noexcept {
  m_initialStateDataFactory = impl;
}

winrt::Windows::Foundation::IInspectable ReactCompositionViewComponentBuilder::InitialStateData(
    winrt::Microsoft::ReactNative::IComponentProps props) noexcept {
  if (m_initialStateDataFactory) {
    return m_initialStateDataFactory(props);
  }

  return nullptr;
}

// (ShadowNode shadowNode, LayoutContext layoutContext) => void
void ReactCompositionViewComponentBuilder::SetLayoutHandler(
    winrt::Microsoft::ReactNative::LayoutHandler impl) noexcept {
  m_layoutHandler = impl;
}

winrt::Microsoft::ReactNative::ComponentView ReactCompositionViewComponentBuilder::CreateView(
    const IReactContext &reactContext,
    int32_t tag,
    const ICompositionContext &context) noexcept {
  if (m_createView) {
    auto args = winrt::make<implementation::CreateCompositionComponentViewArgs>(reactContext, tag, context);
    return m_createView(args);
  } else {
    assert(m_createComponentView);
    auto args = winrt::make<winrt::Microsoft::ReactNative::implementation::CreateComponentViewArgs>(reactContext, tag);
    return m_createComponentView(args);
  }
}

bool ReactCompositionViewComponentBuilder::IsViewComponent() const noexcept {
  return m_createView != nullptr;
}

} // namespace winrt::Microsoft::ReactNative::Composition
