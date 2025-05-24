
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/renderer/textlayoutmanager/WindowsTextLayoutManager.h>

#include <Fabric/Composition/ReactCompositionViewComponentBuilder.h>
#include <ReactPropertyBag.h>
#include <react/renderer/componentregistry/ComponentDescriptorProviderRegistry.h>

namespace Microsoft::ReactNative {

struct WindowsComponentDescriptorRegistry {
  WindowsComponentDescriptorRegistry();

  void Add(
      winrt::hstring componentName,
      winrt::Microsoft::ReactNative::ReactViewComponentProvider const &provider) noexcept;
  std::shared_ptr<facebook::react::ComponentDescriptorProviderRegistry> GetProviderRegistry() const noexcept;

  winrt::Microsoft::ReactNative::IReactViewComponentBuilder GetDescriptor(
      std::shared_ptr<std::string const> &flavor) const noexcept;

  winrt::Microsoft::ReactNative::IReactViewComponentBuilder GetDescriptor(
      facebook::react::ComponentHandle handle) const noexcept;

  static void AddToProperties(
      const winrt::Microsoft::ReactNative::ReactPropertyBag &props,
      std::shared_ptr<WindowsComponentDescriptorRegistry> &registry) noexcept;
  static std::shared_ptr<WindowsComponentDescriptorRegistry> FromProperties(
      const winrt::Microsoft::ReactNative::ReactPropertyBag &props) noexcept;

  bool hasComponentProvider(const std::string &name) noexcept;

 private:
  void add(const facebook::react::ComponentDescriptorProvider &provider) noexcept;

  std::vector<std::shared_ptr<const std::string>> m_descriptorFlavors;
  std::vector<std::string> m_componentNames;
  std::shared_ptr<facebook::react::ComponentDescriptorProviderRegistry> m_componentDescriptorRegistry;

  std::map<std::shared_ptr<const std::string>, winrt::Microsoft::ReactNative::IReactViewComponentBuilder const>
      m_builderByName;
  std::map<facebook::react::ComponentHandle, winrt::Microsoft::ReactNative::IReactViewComponentBuilder const>
      m_builderByHandle;
};

} // namespace Microsoft::ReactNative
