// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "ComponentViewRegistry.h"

#pragma warning(push)
#pragma warning(disable : 4244 4305)
#include <react/renderer/components/view/ViewShadowNode.h>
#include <react/renderer/components/text/ParagraphShadowNode.h>
#include <react/renderer/components/text/TextShadowNode.h>
#include <react/renderer/components/scrollview/ScrollViewShadowNode.h>
#pragma warning(pop)

#include "ParagraphComponentView.h"
#include "ScrollViewComponentView.h"
#include "TextComponentView.h"
#include "ViewComponentView.h"

namespace Microsoft::ReactNative {

ComponentViewDescriptor const &ComponentViewRegistry::dequeueComponentViewWithComponentHandle(
    facebook::react::ComponentHandle componentHandle,
    facebook::react::Tag tag) noexcept {

  // TODO implement recycled components like core does

  std::shared_ptr<BaseComponentView> view;

  if (componentHandle == facebook::react::TextShadowNode::Handle()) {
    view = std::make_shared<TextComponentView>();
  } else if (componentHandle == facebook::react::ParagraphShadowNode::Handle()) {
    view = std::make_shared<ParagraphComponentView>();
  } else if (componentHandle == facebook::react::ScrollViewShadowNode::Handle()) {
    view = std::make_shared<ScrollViewComponentView>();
  }

  else {
    // Currently we are treating a lot of component just as normal views...
    view = std::make_shared<ViewComponentView>();
  }

  view->Element().Tag(winrt::box_value(tag));
  auto it = m_registry.insert({tag, ComponentViewDescriptor{view, false, false}});
  return it.first->second;

}

ComponentViewDescriptor const &ComponentViewRegistry::componentViewDescriptorWithTag(
    facebook::react::Tag tag) noexcept {
  auto iterator = m_registry.find(tag);
  assert(iterator != m_registry.end());
  return iterator->second;
}

void ComponentViewRegistry::enqueueComponentViewWithComponentHandle(
    facebook::react::ComponentHandle componentHandle,
    facebook::react::Tag tag,
    ComponentViewDescriptor componentViewDescriptor) noexcept {
  assert(m_registry.find(tag) != m_registry.end());

  m_registry.erase(tag);
  static_cast<ViewComponentView &>(*componentViewDescriptor.view).Element().Tag(nullptr);
}
} // namespace Microsoft::ReactNative
