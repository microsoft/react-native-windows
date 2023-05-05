
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "WindowsComponentDescriptorRegistry.h"

#include <Fabric/AbiViewComponentDescriptor.h>
#include <Fabric/Composition/TextInput/WindowsTextInputComponentDescriptor.h>
#include <react/components/rnwcore/ComponentDescriptors.h>
#include <react/renderer/components/image/ImageComponentDescriptor.h>
#include <react/renderer/components/text/ParagraphComponentDescriptor.h>
#include <react/renderer/components/text/RawTextComponentDescriptor.h>
#include <react/renderer/components/text/TextComponentDescriptor.h>
#include <react/renderer/components/view/ViewComponentDescriptor.h>

#pragma warning(push)
#pragma warning(disable : 4305)
#include <react/renderer/components/scrollview/ScrollViewComponentDescriptor.h>
#pragma warning(pop)

namespace Microsoft::ReactNative {

winrt::Microsoft::ReactNative::ReactPropertyId<
    winrt::Microsoft::ReactNative::ReactNonAbiValue<std::shared_ptr<WindowsComponentDescriptorRegistry>>>
WindowsComponentDescriptorRegistryProperty() noexcept {
  winrt::Microsoft::ReactNative::ReactPropertyId<
      winrt::Microsoft::ReactNative::ReactNonAbiValue<std::shared_ptr<WindowsComponentDescriptorRegistry>>>
      propId{L"ReactNative.Fabric", L"WindowsComponentDescriptorRegistry"};
  return propId;
}

WindowsComponentDescriptorRegistry::WindowsComponentDescriptorRegistry()
    : m_componentDescriptorRegistry(std::make_shared<facebook::react::ComponentDescriptorProviderRegistry>()) {
  m_componentDescriptorRegistry->add(facebook::react::concreteComponentDescriptorProvider<
                                     facebook::react::ActivityIndicatorViewComponentDescriptor>());

  m_componentDescriptorRegistry->add(
      facebook::react::concreteComponentDescriptorProvider<facebook::react::ImageComponentDescriptor>());
  m_componentDescriptorRegistry->add(
      facebook::react::concreteComponentDescriptorProvider<facebook::react::ParagraphComponentDescriptor>());
  m_componentDescriptorRegistry->add(
      facebook::react::concreteComponentDescriptorProvider<facebook::react::RawTextComponentDescriptor>());
  m_componentDescriptorRegistry->add(
      facebook::react::concreteComponentDescriptorProvider<facebook::react::ScrollViewComponentDescriptor>());
  m_componentDescriptorRegistry->add(
      facebook::react::concreteComponentDescriptorProvider<facebook::react::SwitchComponentDescriptor>());
  m_componentDescriptorRegistry->add(
      facebook::react::concreteComponentDescriptorProvider<facebook::react::TextComponentDescriptor>());
  m_componentDescriptorRegistry->add(
      facebook::react::concreteComponentDescriptorProvider<facebook::react::ViewComponentDescriptor>());
  m_componentDescriptorRegistry->add(
      facebook::react::concreteComponentDescriptorProvider<facebook::react::WindowsTextInputComponentDescriptor>());
}

void WindowsComponentDescriptorRegistry::Add(
    winrt::hstring componentName,
    winrt::Microsoft::ReactNative::ReactViewComponentProvider const &provider) noexcept {
  auto builder = winrt::make<winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder>();
  provider(builder);

  m_descriptorFlavors.emplace_back(std::make_shared<std::string>(winrt::to_string(componentName)));
  auto handle = reinterpret_cast<facebook::react::ComponentHandle>(
      facebook::react::ComponentName(m_descriptorFlavors.back()->c_str()));
  m_builderByName.emplace(m_descriptorFlavors.back(), builder);
  m_builderByHandle.emplace(handle, builder);
  m_componentDescriptorRegistry->add(
      {handle,
       m_descriptorFlavors.back()->c_str(),
       std::static_pointer_cast<void const>(m_descriptorFlavors.back()),
       &facebook::react::concreteComponentDescriptorConstructor<AbiViewComponentDescriptor>});
}

winrt::Microsoft::ReactNative::IReactViewComponentBuilder WindowsComponentDescriptorRegistry::GetDescriptor(
    std::shared_ptr<std::string const> &flavor) const noexcept {
  return m_builderByName.at(flavor);
}

winrt::Microsoft::ReactNative::IReactViewComponentBuilder WindowsComponentDescriptorRegistry::GetDescriptor(
    facebook::react::ComponentHandle handle) const noexcept {
  return m_builderByHandle.at(handle);
}

std::shared_ptr<facebook::react::ComponentDescriptorProviderRegistry>
WindowsComponentDescriptorRegistry::GetProviderRegistry() const noexcept {
  return m_componentDescriptorRegistry;
}

/*static*/ void WindowsComponentDescriptorRegistry::AddToProperties(
    const winrt::Microsoft::ReactNative::ReactPropertyBag &props,
    std::shared_ptr<WindowsComponentDescriptorRegistry> &registry) noexcept {
  props.Set(WindowsComponentDescriptorRegistryProperty(), registry);
}

/*static*/ std::shared_ptr<WindowsComponentDescriptorRegistry> WindowsComponentDescriptorRegistry::FromProperties(
    const winrt::Microsoft::ReactNative::ReactPropertyBag &props) noexcept {
  return props.Get(WindowsComponentDescriptorRegistryProperty()).Value();
}

} // namespace Microsoft::ReactNative
