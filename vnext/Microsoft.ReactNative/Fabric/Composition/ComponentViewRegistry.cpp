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

#include <Fabric/Composition/CompositionHelpers.h>
#include <Fabric/Composition/CompositionViewComponentView.h>
#include <Fabric/Composition/ImageComponentView.h>
#include <Fabric/Composition/ParagraphComponentView.h>
#include <Fabric/Composition/ScrollViewComponentView.h>
#include <Fabric/Composition/TextInput/WindowsTextInputComponentView.h>

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

  if (componentHandle == facebook::react::ParagraphShadowNode::Handle()) {
    view = std::make_shared<ParagraphComponentView>(compContext, tag);
  } else if (componentHandle == facebook::react::ScrollViewShadowNode::Handle()) {
    view = std::make_shared<ScrollViewComponentView>(compContext, tag);
  } else if (componentHandle == facebook::react::ImageShadowNode::Handle()) {
    view = std::make_shared<ImageComponentView>(compContext, tag, m_context);
  } else if (componentHandle == facebook::react::WindowsTextInputShadowNode::Handle()) {
    view = std::make_shared<WindowsTextInputComponentView>(compContext, tag, m_context);
  } else {
    view = std::make_shared<CompositionViewComponentView>(compContext, tag);
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
