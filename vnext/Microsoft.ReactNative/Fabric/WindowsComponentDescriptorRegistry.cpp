
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "WindowsComponentDescriptorRegistry.h"

#include <Fabric/AbiComponentDescriptor.h>
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
  add(facebook::react::concreteComponentDescriptorProvider<
      facebook::react::ActivityIndicatorViewComponentDescriptor>());
  add(facebook::react::concreteComponentDescriptorProvider<facebook::react::DebuggingOverlayComponentDescriptor>());
  add(facebook::react::concreteComponentDescriptorProvider<facebook::react::ImageComponentDescriptor>());
  add(facebook::react::concreteComponentDescriptorProvider<facebook::react::ParagraphComponentDescriptor>());
  add(facebook::react::concreteComponentDescriptorProvider<facebook::react::RawTextComponentDescriptor>());
  add(facebook::react::concreteComponentDescriptorProvider<facebook::react::ScrollViewComponentDescriptor>());
  add(facebook::react::concreteComponentDescriptorProvider<facebook::react::SwitchComponentDescriptor>());
  add(facebook::react::concreteComponentDescriptorProvider<facebook::react::TextComponentDescriptor>());
  add(facebook::react::concreteComponentDescriptorProvider<facebook::react::ViewComponentDescriptor>());
  add(facebook::react::concreteComponentDescriptorProvider<facebook::react::WindowsTextInputComponentDescriptor>());
}

void WindowsComponentDescriptorRegistry::add(const facebook::react::ComponentDescriptorProvider &provider) noexcept {
  m_componentNames.push_back(provider.name);
  m_componentDescriptorRegistry->add(provider);
}

bool WindowsComponentDescriptorRegistry::hasComponentProvider(const std::string &name) noexcept {
  return std::find(m_componentNames.begin(), m_componentNames.end(), name) != m_componentNames.end();
}

bool WindowsComponentDescriptorRegistry::isXamlSupportRequired() const noexcept {
  return std::any_of(m_builderByName.cbegin(), m_builderByName.cend(), [](const auto& pair) -> bool {
      return winrt::get_self<winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder>(pair.second)
        ->XamlSupport();
    });
}

void WindowsComponentDescriptorRegistry::Add(
    winrt::hstring componentName,
    winrt::Microsoft::ReactNative::ReactViewComponentProvider const &provider) noexcept {
  auto builder = winrt::make<winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder>();
  provider(builder);

  m_componentNames.push_back(winrt::to_string(componentName));
  m_descriptorFlavors.emplace_back(std::make_shared<std::string>(winrt::to_string(componentName)));
  auto handle = reinterpret_cast<facebook::react::ComponentHandle>(
      facebook::react::ComponentName(m_descriptorFlavors.back()->c_str()));
  m_builderByName.emplace(m_descriptorFlavors.back(), builder);
  m_builderByHandle.emplace(handle, builder);

  m_componentDescriptorRegistry->add(
      {handle,
       m_descriptorFlavors.back()->c_str(),
       std::static_pointer_cast<void const>(m_descriptorFlavors.back()),
       winrt::get_self<winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder>(builder)
           ->GetComponentDescriptorProvider()});
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
