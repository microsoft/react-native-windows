// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "ComponentViewRegistry.h"

#pragma warning(push)
#pragma warning(disable : 4305)
#include <react/renderer/components/scrollview/ScrollViewShadowNode.h>
#pragma warning(pop)

#include <Fabric/Composition/ActivityIndicatorComponentView.h>
#include <Fabric/Composition/CompositionHelpers.h>
#include <Fabric/Composition/CompositionViewComponentView.h>
#include <Fabric/Composition/DebuggingOverlayComponentView.h>
#include <Fabric/Composition/ImageComponentView.h>
#include <Fabric/Composition/Modal/WindowsModalHostViewComponentView.h>
#include <Fabric/Composition/Modal/WindowsModalHostViewShadowNode.h>
#include <Fabric/Composition/ParagraphComponentView.h>
#include <Fabric/Composition/RootComponentView.h>
#include <Fabric/Composition/ScrollViewComponentView.h>
#include <Fabric/Composition/SwitchComponentView.h>
#include <Fabric/Composition/TextInput/WindowsTextInputComponentView.h>
#include <Fabric/Composition/TextInput/WindowsTextInputShadowNode.h>
#include <Fabric/Composition/UnimplementedNativeViewComponentView.h>
#include <Fabric/WindowsComponentDescriptorRegistry.h>
#include <react/components/rnwcore/ShadowNodes.h>
#include <react/renderer/components/image/ImageShadowNode.h>
#include <react/renderer/components/root/RootShadowNode.h>
#include <react/renderer/components/text/ParagraphShadowNode.h>
#include <react/renderer/components/text/RawTextShadowNode.h>
#include <react/renderer/components/text/TextShadowNode.h>
#include <react/renderer/components/view/ViewShadowNode.h>

namespace Microsoft::ReactNative {

void ComponentViewRegistry::Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

ComponentViewDescriptor const &ComponentViewRegistry::dequeueComponentViewWithComponentHandle(
    facebook::react::ComponentHandle componentHandle,
    facebook::react::Tag tag,
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext) noexcept {
  // TODO implement recycled components like core does

  winrt::Microsoft::ReactNative::ComponentView view{nullptr};

  if (componentHandle == facebook::react::ViewShadowNode::Handle()) {
    view = winrt::Microsoft::ReactNative::Composition::implementation::ViewComponentView::Create(
        compContext, tag, m_context);
  } else if (componentHandle == facebook::react::ParagraphShadowNode::Handle()) {
    view = winrt::Microsoft::ReactNative::Composition::implementation::ParagraphComponentView::Create(
        compContext, tag, m_context);
  } else if (componentHandle == facebook::react::ScrollViewShadowNode::Handle()) {
    view = winrt::Microsoft::ReactNative::Composition::implementation::ScrollViewComponentView::Create(
        compContext, tag, m_context);
  } else if (componentHandle == facebook::react::ImageShadowNode::Handle()) {
    view = winrt::Microsoft::ReactNative::Composition::implementation::ImageComponentView::Create(
        compContext, tag, m_context);
  } else if (componentHandle == facebook::react::WindowsModalHostViewShadowNode::Handle()) {
    view = winrt::Microsoft::ReactNative::Composition::implementation::WindowsModalHostComponentView::Create(
        compContext, tag, m_context);
  } else if (componentHandle == facebook::react::WindowsTextInputShadowNode::Handle()) {
    view = winrt::Microsoft::ReactNative::Composition::implementation::WindowsTextInputComponentView::Create(
        compContext, tag, m_context);
  } else if (componentHandle == facebook::react::SwitchShadowNode::Handle()) {
    view = winrt::Microsoft::ReactNative::Composition::implementation::SwitchComponentView::Create(
        compContext, tag, m_context);
  } else if (componentHandle == facebook::react::ActivityIndicatorViewShadowNode::Handle()) {
    view = winrt::Microsoft::ReactNative::Composition::implementation::ActivityIndicatorComponentView::Create(
        compContext, tag, m_context);
  } else if (componentHandle == facebook::react::RootShadowNode::Handle()) {
    view = winrt::Microsoft::ReactNative::Composition::implementation::RootComponentView::Create(
        compContext, tag, m_context);
  } else if (
      componentHandle == facebook::react::RawTextShadowNode::Handle() ||
      componentHandle == facebook::react::TextShadowNode::Handle()) {
    // Review - Why do we get asked for ComponentViews for Text/RawText... do these get used?
    view = winrt::Microsoft::ReactNative::Composition::implementation::ViewComponentView::Create(
        compContext, tag, m_context);
  } else if (componentHandle == facebook::react::UnimplementedNativeViewShadowNode::Handle()) {
    view = winrt::Microsoft::ReactNative::Composition::implementation::UnimplementedNativeViewComponentView::Create(
        compContext, tag, m_context);
  } else if (componentHandle == facebook::react::DebuggingOverlayShadowNode::Handle()) {
    view = winrt::Microsoft::ReactNative::Composition::implementation::DebuggingOverlayComponentView::Create(
        compContext, tag, m_context);
  } else {
    auto descriptor =
        WindowsComponentDescriptorRegistry::FromProperties(m_context.Properties())->GetDescriptor(componentHandle);
    view = winrt::get_self<winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder>(
               descriptor.as<winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder>())
               ->CreateView(m_context.Handle(), tag, compContext);
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

winrt::Microsoft::ReactNative::ComponentView ComponentViewRegistry::findComponentViewWithTag(
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

  winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(componentViewDescriptor.view)
      ->prepareForRecycle();

  m_registry.erase(tag);

  winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(componentViewDescriptor.view)
      ->onDestroying();
}
} // namespace Microsoft::ReactNative
