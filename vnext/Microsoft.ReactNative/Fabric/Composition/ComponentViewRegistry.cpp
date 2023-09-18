// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "ComponentViewRegistry.h"

#pragma warning(push)
#pragma warning(disable : 4305)
#include <react/renderer/components/scrollview/ScrollViewShadowNode.h>
#pragma warning(pop)

#include <Fabric/Composition/TextInput/WindowsTextInputShadowNode.h>
#include <react/components/rnwcore/ShadowNodes.h>
#include <react/renderer/components/image/ImageShadowNode.h>
#include <react/renderer/components/root/RootShadowNode.h>
#include <react/renderer/components/text/ParagraphShadowNode.h>
#include <react/renderer/components/text/RawTextShadowNode.h>
#include <react/renderer/components/text/TextShadowNode.h>
#include <react/renderer/components/view/ViewShadowNode.h>

#include <Fabric/Composition/AbiCompositionViewComponentView.h>
#include <Fabric/Composition/ActivityIndicatorComponentView.h>
#include <Fabric/Composition/CompositionHelpers.h>
#include <Fabric/Composition/CompositionViewComponentView.h>
#include <Fabric/Composition/ImageComponentView.h>
#include <Fabric/Composition/ParagraphComponentView.h>
#include <Fabric/Composition/RootComponentView.h>
#include <Fabric/Composition/ScrollViewComponentView.h>
#include <Fabric/Composition/SwitchComponentView.h>
#include <Fabric/Composition/TextInput/WindowsTextInputComponentView.h>
#include <Fabric/Composition/UnimplementedNativeViewComponentView.h>
#include <Fabric/WindowsComponentDescriptorRegistry.h>

namespace Microsoft::ReactNative {

void ComponentViewRegistry::Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

ComponentViewDescriptor const &ComponentViewRegistry::dequeueComponentViewWithComponentHandle(
    facebook::react::ComponentHandle componentHandle,
    facebook::react::Tag tag,
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext) noexcept {
  // TODO implement recycled components like core does

  std::shared_ptr<CompositionBaseComponentView> view;

  if (componentHandle == facebook::react::ViewShadowNode::Handle()) {
    view = CompositionViewComponentView::Create(compContext, tag);
  } else if (componentHandle == facebook::react::ParagraphShadowNode::Handle()) {
    view = ParagraphComponentView::Create(compContext, tag);
  } else if (componentHandle == facebook::react::ScrollViewShadowNode::Handle()) {
    view = ScrollViewComponentView::Create(compContext, tag);
  } else if (componentHandle == facebook::react::ImageShadowNode::Handle()) {
    view = ImageComponentView::Create(compContext, tag, m_context);
  } else if (componentHandle == facebook::react::WindowsTextInputShadowNode::Handle()) {
    view = WindowsTextInputComponentView::Create(compContext, tag, m_context);
  } else if (componentHandle == facebook::react::SwitchShadowNode::Handle()) {
    view = SwitchComponentView::Create(compContext, tag, m_context);
  } else if (componentHandle == facebook::react::ActivityIndicatorViewShadowNode::Handle()) {
    view = ActivityIndicatorComponentView::Create(compContext, tag, m_context);
  } else if (componentHandle == facebook::react::RootShadowNode::Handle()) {
    view = RootComponentView::Create(compContext, tag, m_context);
  } else if (
      componentHandle == facebook::react::RawTextShadowNode::Handle() ||
      componentHandle == facebook::react::TextShadowNode::Handle()) {
    // Review - Why do we get asked for ComponentViews for Text/RawText... do these get used?
    view = CompositionViewComponentView::Create(compContext, tag);
  } else if (componentHandle == facebook::react::UnimplementedNativeViewShadowNode::Handle()) {
    view = UnimplementedNativeViewComponentView::Create(compContext, tag);
  } else {
    auto descriptor =
        WindowsComponentDescriptorRegistry::FromProperties(m_context.Properties())->GetDescriptor(componentHandle);
    view = AbiCompositionViewComponentView::Create(
        m_context.Handle(),
        compContext,
        tag,
        descriptor.as<winrt::Microsoft::ReactNative::IReactViewComponentBuilder>());
  }

  auto it = m_registry.insert({tag, ComponentViewDescriptor{view}});
  return it.first->second;
}

ComponentViewDescriptor const &ComponentViewRegistry::componentViewDescriptorWithTag(
    facebook::react::Tag tag) const noexcept {
  auto iterator = m_registry.find(tag);
  assert(iterator != m_registry.end());
  return iterator->second;
}

std::shared_ptr<IComponentView> ComponentViewRegistry::findComponentViewWithTag(
    facebook::react::Tag tag) const noexcept {
  auto iterator = m_registry.find(tag);
  if (iterator == m_registry.end()) {
    return nullptr;
  }
  return iterator->second.view;
}

void ComponentViewRegistry::enqueueComponentViewWithComponentHandle(
    facebook::react::ComponentHandle componentHandle,
    facebook::react::Tag tag,
    ComponentViewDescriptor componentViewDescriptor) noexcept {
  assert(m_registry.find(tag) != m_registry.end());

  m_registry.erase(tag);
}
} // namespace Microsoft::ReactNative
