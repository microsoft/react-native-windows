// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactCompositionViewComponentBuilder.h"
#include <Fabric/Composition/CompositionViewComponentView.h>
#include <Fabric/Composition/ContentIslandComponentView.h>
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

void ReactCompositionViewComponentBuilder::InitializeComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
  auto self = winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(view);
  self->MarkAsCustomComponent();
  if (m_customCommandHandler)
    self->CustomCommandHandler(m_customCommandHandler);
  if (m_finalizeUpdateHandler)
    self->FinalizeUpdateHandler(m_finalizeUpdateHandler);
  if (m_updatePropsHandler)
    self->UpdatePropsHandler(m_updatePropsHandler);
  if (m_updateStateHandler)
    self->UpdateStateHandler(m_updateStateHandler);
  if (m_updateEventEmitterHandler)
    self->UpdateEventEmitterHandler(m_updateEventEmitterHandler);
  if (m_mountChildComponentViewHandler)
    self->MountChildComponentViewHandler(m_mountChildComponentViewHandler);
  if (m_unmountChildComponentViewHandler)
    self->UnmountChildComponentViewHandler(m_unmountChildComponentViewHandler);
  if (m_createVisualHandler)
    view.as<winrt::Microsoft::ReactNative::Composition::implementation::ViewComponentView>()->CreateVisualHandler(
        m_createVisualHandler);
}

void ReactCompositionViewComponentBuilder::SetComponentViewInitializer(
    const ComponentViewInitializer &initializer) noexcept {
  m_fnCreateView =
      [initializer](const IReactContext &reactContext, int32_t tag, const Experimental::ICompositionContext &context) {
        auto view = winrt::make<winrt::Microsoft::ReactNative::implementation::ComponentView>(tag, reactContext);
        initializer(view);
        return view;
      };
  m_descriptorConstructorFactory = []() {
    return &facebook::react::concreteComponentDescriptorConstructor<::Microsoft::ReactNative::AbiComponentDescriptor>;
  };
}

void ReactCompositionViewComponentBuilder::SetViewComponentViewInitializer(
    const ViewComponentViewInitializer &initializer) noexcept {
  m_fnCreateView =
      [initializer](const IReactContext &reactContext, int32_t tag, const Experimental::ICompositionContext &context) {
        auto view = winrt::Microsoft::ReactNative::Composition::implementation::ViewComponentView::Create(
                        context, tag, reactContext)
                        .as<winrt::Microsoft::ReactNative::Composition::ViewComponentView>();
        initializer(view);
        return view;
      };
  m_descriptorConstructorFactory = []() {
    return &facebook::react::concreteComponentDescriptorConstructor<
        ::Microsoft::ReactNative::AbiViewComponentDescriptor>;
  };
}

void ReactCompositionViewComponentBuilder::SetContentIslandComponentViewInitializer(
    const ComponentIslandComponentViewInitializer &initializer) noexcept {
  m_fnCreateView = [initializer](
                       const IReactContext &reactContext, int32_t tag, const Experimental::ICompositionContext &context)
      -> winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView {
    auto view = winrt::make<winrt::Microsoft::ReactNative::Composition::implementation::ContentIslandComponentView>(
        context, tag, reactContext);
    initializer(view);
    return view;
  };
  m_descriptorConstructorFactory = []() {
    return &facebook::react::concreteComponentDescriptorConstructor<
        ::Microsoft::ReactNative::AbiViewComponentDescriptor>;
  };
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

void ReactCompositionViewComponentBuilder::SetCustomCommandHandler(HandleCommandDelegate impl) noexcept {
  m_customCommandHandler = impl;
}

void ReactCompositionViewComponentBuilder::SetFinalizeUpdateHandler(UpdateFinalizerDelegate impl) noexcept {
  m_finalizeUpdateHandler = impl;
}

void ReactCompositionViewComponentBuilder::SetUpdatePropsHandler(UpdatePropsDelegate impl) noexcept {
  m_updatePropsHandler = impl;
}

void ReactCompositionViewComponentBuilder::SetUpdateStateHandler(UpdateStateDelegate impl) noexcept {
  m_updateStateHandler = impl;
}

void ReactCompositionViewComponentBuilder::SetUpdateEventEmitterHandler(UpdateEventEmitterDelegate impl) noexcept {
  m_updateEventEmitterHandler = impl;
}

void ReactCompositionViewComponentBuilder::SetMountChildComponentViewHandler(
    MountChildComponentViewDelegate impl) noexcept {
  m_mountChildComponentViewHandler = impl;
}

void ReactCompositionViewComponentBuilder::SetUnmountChildComponentViewHandler(
    UnmountChildComponentViewDelegate impl) noexcept {
  m_unmountChildComponentViewHandler = impl;
}

void ReactCompositionViewComponentBuilder::SetCreateVisualHandler(CreateVisualDelegate impl) noexcept {
  m_createVisualHandler = impl;
}

winrt::Microsoft::ReactNative::ComponentView ReactCompositionViewComponentBuilder::CreateView(
    const IReactContext &reactContext,
    int32_t tag,
    const Experimental::ICompositionContext &context) noexcept {
  assert(m_fnCreateView);
  auto view = m_fnCreateView(reactContext, tag, context);
  InitializeComponentView(view);
  return view;
}

facebook::react::ComponentDescriptorConstructor *ReactCompositionViewComponentBuilder::GetComponentDescriptorProvider()
    const noexcept {
  return m_descriptorConstructorFactory();
}

} // namespace winrt::Microsoft::ReactNative::Composition
