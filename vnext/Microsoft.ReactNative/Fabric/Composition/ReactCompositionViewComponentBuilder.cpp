// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactCompositionViewComponentBuilder.h"
#include <Fabric/AbiViewComponentDescriptor.h>
#include <Fabric/Composition/CompositionViewComponentView.h>
#include <Fabric/Composition/ContentIslandComponentView.h>
#include <Fabric/Composition/PortalComponentView.h>
#include <strsafe.h>
#include "CompositionContextHelper.h"
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

IComponentProps ReactCompositionViewComponentBuilder::CreateProps(
    ViewProps props,
    const IComponentProps &cloneFrom) noexcept {
  return m_propsFactory(props, cloneFrom);
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
}

void ReactCompositionViewComponentBuilder::SetComponentViewInitializer(
    const ComponentViewInitializer &initializer) noexcept {
  m_fnCreateView = [initializer](
                       const IReactContext &reactContext,
                       int32_t tag,
                       const Experimental::ICompositionContext &context,
                       ComponentViewFeatures,
                       ReactCompositionViewComponentBuilder &builder) {
    auto view = winrt::make<winrt::Microsoft::ReactNative::implementation::ComponentView>(tag, reactContext, &builder);
    initializer(view);
    return view;
  };
  m_descriptorConstructorFactory = []() {
    return &facebook::react::concreteComponentDescriptorConstructor<::Microsoft::ReactNative::AbiComponentDescriptor>;
  };
}

void ReactCompositionViewComponentBuilder::SetViewComponentViewInitializer(
    const ViewComponentViewInitializer &initializer) noexcept {
  m_fnCreateView = [initializer](
                       const IReactContext &reactContext,
                       int32_t tag,
                       const Experimental::ICompositionContext &context,
                       ComponentViewFeatures features,
                       ReactCompositionViewComponentBuilder &builder) {
    auto view = winrt::make<implementation::ViewComponentView>(
        implementation::ViewComponentView::defaultProps(), context, tag, reactContext, features, &builder);
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
                       const IReactContext &reactContext,
                       int32_t tag,
                       const Experimental::ICompositionContext &context,
                       ComponentViewFeatures /*features*/,
                       ReactCompositionViewComponentBuilder &builder)
      -> winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView {
    auto view = winrt::make<winrt::Microsoft::ReactNative::Composition::implementation::ContentIslandComponentView>(
        context, tag, reactContext, &builder);
    initializer(view);
    return view;
  };
  m_descriptorConstructorFactory = []() {
    return &facebook::react::concreteComponentDescriptorConstructor<
        ::Microsoft::ReactNative::AbiViewComponentDescriptor>;
  };
}

void ReactCompositionViewComponentBuilder::SetPortalComponentViewInitializer(
    const PortalComponentViewInitializer &initializer) noexcept {
  m_fnCreateView = [initializer](
                       const IReactContext &reactContext,
                       int32_t tag,
                       const Experimental::ICompositionContext &context,
                       ComponentViewFeatures /*features*/,
                       ReactCompositionViewComponentBuilder &builder)
      -> winrt::Microsoft::ReactNative::Composition::PortalComponentView {
    auto view = winrt::make<winrt::Microsoft::ReactNative::Composition::implementation::PortalComponentView>(
        context, tag, reactContext, &builder);
    initializer(view);
    return view;
  };
  m_descriptorConstructorFactory = []() {
    return &facebook::react::concreteComponentDescriptorConstructor<
        ::Microsoft::ReactNative::AbiPortalComponentDescriptor>;
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

const HandleCommandDelegate &ReactCompositionViewComponentBuilder::CustomCommandHandler() const noexcept {
  return m_customCommandHandler;
}

void ReactCompositionViewComponentBuilder::SetFinalizeUpdateHandler(UpdateFinalizerDelegate impl) noexcept {
  m_finalizeUpdateHandler = impl;
}

const UpdateFinalizerDelegate &ReactCompositionViewComponentBuilder::FinalizeUpdateHandler() const noexcept {
  return m_finalizeUpdateHandler;
}

void ReactCompositionViewComponentBuilder::SetUpdatePropsHandler(UpdatePropsDelegate impl) noexcept {
  m_updatePropsHandler = impl;
}

const winrt::Microsoft::ReactNative::UpdatePropsDelegate &ReactCompositionViewComponentBuilder::UpdatePropsHandler()
    const noexcept {
  return m_updatePropsHandler;
}

void ReactCompositionViewComponentBuilder::SetUpdateStateHandler(UpdateStateDelegate impl) noexcept {
  m_updateStateHandler = impl;
}

const winrt::Microsoft::ReactNative::UpdateStateDelegate &ReactCompositionViewComponentBuilder::UpdateStateHandler()
    const noexcept {
  return m_updateStateHandler;
}

void ReactCompositionViewComponentBuilder::SetUpdateEventEmitterHandler(UpdateEventEmitterDelegate impl) noexcept {
  m_updateEventEmitterHandler = impl;
}

const UpdateEventEmitterDelegate &ReactCompositionViewComponentBuilder::UpdateEventEmitterHandler() const noexcept {
  return m_updateEventEmitterHandler;
}

void ReactCompositionViewComponentBuilder::SetMountChildComponentViewHandler(
    MountChildComponentViewDelegate impl) noexcept {
  m_mountChildComponentViewHandler = impl;
}

const MountChildComponentViewDelegate &ReactCompositionViewComponentBuilder::MountChildComponentViewHandler()
    const noexcept {
  return m_mountChildComponentViewHandler;
}

void ReactCompositionViewComponentBuilder::SetUnmountChildComponentViewHandler(
    UnmountChildComponentViewDelegate impl) noexcept {
  m_unmountChildComponentViewHandler = impl;
}

const UnmountChildComponentViewDelegate &ReactCompositionViewComponentBuilder::UnmountChildComponentViewHandler()
    const noexcept {
  return m_unmountChildComponentViewHandler;
}

void ReactCompositionViewComponentBuilder::SetCreateVisualHandler(CreateVisualDelegate impl) noexcept {
  m_createVisualHandler = impl;
}

const CreateVisualDelegate &ReactCompositionViewComponentBuilder::CreateVisualHandler() const noexcept {
  return m_createVisualHandler;
}

void ReactCompositionViewComponentBuilder::SetViewFeatures(ComponentViewFeatures viewFeatures) noexcept {
  m_features = viewFeatures;
}

void ReactCompositionViewComponentBuilder::SetVisualToMountChildrenIntoHandler(
    VisualToMountChildrenIntoDelegate impl) noexcept {
  m_visualToMountChildrenIntoHandler = [impl](const winrt::Microsoft::ReactNative::ComponentView &view) {
    return winrt::Microsoft::ReactNative::Composition::Experimental::implementation::MicrosoftCompositionContextHelper::
        CreateVisual(impl(view));
  };
}

void ReactCompositionViewComponentBuilder::SetIVisualToMountChildrenIntoHandler(
    winrt::Microsoft::ReactNative::Composition::Experimental::IVisualToMountChildrenIntoDelegate impl) noexcept {
  m_visualToMountChildrenIntoHandler = impl;
}

const winrt::Microsoft::ReactNative::Composition::Experimental::IVisualToMountChildrenIntoDelegate &
ReactCompositionViewComponentBuilder::VisualToMountChildrenIntoHandler() const noexcept {
  return m_visualToMountChildrenIntoHandler;
}

void ReactCompositionViewComponentBuilder::SetUpdateLayoutMetricsHandler(UpdateLayoutMetricsDelegate impl) noexcept {
  m_updateLayoutMetricsHandler = impl;
}

const UpdateLayoutMetricsDelegate &ReactCompositionViewComponentBuilder::UpdateLayoutMetricsHandler() const noexcept {
  return m_updateLayoutMetricsHandler;
}

winrt::Microsoft::ReactNative::ComponentView ReactCompositionViewComponentBuilder::CreateView(
    const IReactContext &reactContext,
    int32_t tag,
    const Experimental::ICompositionContext &context) noexcept {
  assert(m_fnCreateView);
  auto view = m_fnCreateView(reactContext, tag, context, m_features, *this);
  InitializeComponentView(view);
  return view;
}

facebook::react::ComponentDescriptorConstructor *ReactCompositionViewComponentBuilder::GetComponentDescriptorProvider()
    const noexcept {
  return m_descriptorConstructorFactory();
}

} // namespace winrt::Microsoft::ReactNative::Composition
