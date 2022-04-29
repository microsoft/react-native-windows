// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "CompComponentViewRegistry.h"

#pragma warning(push)
#pragma warning(disable : 4305)
#include <react/renderer/components/scrollview/ScrollViewShadowNode.h>
#pragma warning(pop)

#include <Fabric/WinComp/TextInput/CompWindowsTextInputShadowNode.h>
#include <react/components/rnwcore/ShadowNodes.h>
#include <react/renderer/components/image/ImageShadowNode.h>
#include <react/renderer/components/root/RootShadowNode.h>
#include <react/renderer/components/text/ParagraphShadowNode.h>
#include <react/renderer/components/text/RawTextShadowNode.h>
#include <react/renderer/components/text/TextShadowNode.h>
#include <react/renderer/components/view/ViewShadowNode.h>

#include <Fabric/WinComp/CompHelpers.h>
#include <Fabric/WinComp/CompImageComponentView.h>
#include <Fabric/WinComp/CompParagraphComponentView.h>
#include <Fabric/WinComp/CompScrollViewComponentView.h>
#include <Fabric/WinComp/CompViewComponentView.h>
#include <Fabric/WinComp/TextInput/CompWindowsTextInputComponentView.h>

namespace Microsoft::ReactNative {

void CompComponentViewRegistry::Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

ComponentViewDescriptor const &CompComponentViewRegistry::dequeueComponentViewWithComponentHandle(
    facebook::react::ComponentHandle componentHandle,
    facebook::react::Tag tag,
    const std::shared_ptr<CompContext> &compContext) noexcept {
  // TODO implement recycled components like core does

  std::shared_ptr<CompBaseComponentView> view;

  if (componentHandle == facebook::react::ParagraphShadowNode::Handle()) {
    view = std::make_shared<CompParagraphComponentView>();
  } else if (componentHandle == facebook::react::ScrollViewShadowNode::Handle()) {
    view = std::make_shared<CompScrollViewComponentView>();
  } else if (componentHandle == facebook::react::ImageShadowNode::Handle()) {
    view = std::make_shared<CompImageComponentView>(m_context);
  } else if (componentHandle == facebook::react::CompWindowsTextInputShadowNode::Handle()) {
    view = std::make_shared<CompWindowsTextInputComponentView>(m_context);
  } else {
    view = std::make_shared<CompViewComponentView>();
  }
  view->CompContext(compContext);
  view->Tag(tag);

  auto it = m_registry.insert({tag, ComponentViewDescriptor{view}});
  return it.first->second;
}

ComponentViewDescriptor const &CompComponentViewRegistry::componentViewDescriptorWithTag(
    facebook::react::Tag tag) const noexcept {
  auto iterator = m_registry.find(tag);
  assert(iterator != m_registry.end());
  return iterator->second;
}

std::shared_ptr<IComponentView> CompComponentViewRegistry::findComponentViewWithTag(
    facebook::react::Tag tag) const noexcept {
  auto iterator = m_registry.find(tag);
  if (iterator == m_registry.end()) {
    return nullptr;
  }
  return iterator->second.view;
}

void CompComponentViewRegistry::enqueueComponentViewWithComponentHandle(
    facebook::react::ComponentHandle componentHandle,
    facebook::react::Tag tag,
    ComponentViewDescriptor componentViewDescriptor) noexcept {
  assert(m_registry.find(tag) != m_registry.end());

  m_registry.erase(tag);
}
} // namespace Microsoft::ReactNative
