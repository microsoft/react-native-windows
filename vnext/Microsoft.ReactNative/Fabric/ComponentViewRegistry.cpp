// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "ComponentViewRegistry.h"

#pragma warning(push)
#pragma warning(disable : 4305)
#include <react/renderer/components/scrollview/ScrollViewShadowNode.h>
#pragma warning(pop)

#include <Fabric/WinComp/TextInput/CompWindowsTextInputShadowNode.h>
#include <react/components/rnwcore/ShadowNodes.h>
#include <react/renderer/components/image/ImageShadowNode.h>
#include <react/renderer/components/root/RootShadowNode.h>
#include <react/renderer/components/slider/SliderShadowNode.h>
#include <react/renderer/components/text/ParagraphShadowNode.h>
#include <react/renderer/components/text/RawTextShadowNode.h>
#include <react/renderer/components/text/TextShadowNode.h>
#include <react/renderer/components/view/ViewShadowNode.h>

#include "ActivityIndicatorComponentView.h"
#include "ImageComponentView.h"
#include "ParagraphComponentView.h"
#include "ScrollViewComponentView.h"
#include "SliderComponentView.h"
#include "SwitchComponentView.h"
#include "TextComponentView.h"
#include "TextInput/WindowsTextInputComponentView.h"
#include "TextInput/WindowsTextInputShadowNode.h"
#include "ViewComponentView.h"
#include "XamlView.h"

namespace Microsoft::ReactNative {

void ComponentViewRegistry::Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

ComponentViewDescriptor const &ComponentViewRegistry::dequeueComponentViewWithComponentHandle(
    facebook::react::ComponentHandle componentHandle,
    facebook::react::Tag tag,
    const std::shared_ptr<CompContext> &compContext) noexcept {
  // TODO implement recycled components like core does

  std::shared_ptr<BaseComponentView> view;

  if (componentHandle == facebook::react::TextShadowNode::Handle()) {
    view = std::make_shared<TextComponentView>();
  } else if (componentHandle == facebook::react::ParagraphShadowNode::Handle()) {
    view = std::make_shared<ParagraphComponentView>();
  } else if (componentHandle == facebook::react::ScrollViewShadowNode::Handle()) {
    view = std::make_shared<ScrollViewComponentView>();
  } else if (componentHandle == facebook::react::ImageShadowNode::Handle()) {
    view = std::make_shared<ImageComponentView>(m_context);
  } else if (componentHandle == facebook::react::SliderShadowNode::Handle()) {
    view = std::make_shared<SliderComponentView>(m_context);
  } else if (componentHandle == facebook::react::SwitchShadowNode::Handle()) {
    view = std::make_shared<SwitchComponentView>(m_context);
  } else if (componentHandle == facebook::react::WindowsTextInputShadowNode::Handle()) {
    view = std::make_shared<WindowsTextInputComponentView>();
  } else if (componentHandle == facebook::react::ActivityIndicatorViewShadowNode::Handle()) {
    view = std::make_shared<ActivityIndicatorComponentView>();
  } else {
    // Just to keep track of what kinds of shadownodes we are being used verify we know about them here
    assert(
        componentHandle == facebook::react::RawTextShadowNode::Handle() ||
        componentHandle == facebook::react::WindowsTextInputShadowNode::Handle() ||
        componentHandle == facebook::react::RootShadowNode::Handle() ||
        componentHandle == facebook::react::ViewShadowNode::Handle());

    view = std::make_shared<ViewComponentView>();
  }

  SetTag(view->Element(), tag);

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
  SetTag(static_cast<ViewComponentView &>(*componentViewDescriptor.view).Element(), InvalidTag);
}
} // namespace Microsoft::ReactNative
