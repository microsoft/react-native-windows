
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/renderer/componentregistry/ComponentDescriptorProviderRegistry.h>
#include <ReactPropertyBag.h>

namespace Microsoft::ReactNative {

struct WindowsComponentDescriptorRegistry {

WindowsComponentDescriptorRegistry();

void Add(winrt::Microsoft::ReactNative::IViewComponentDescriptor const &descriptor) noexcept;
std::shared_ptr<facebook::react::ComponentDescriptorProviderRegistry> GetProviderRegistry() const noexcept;
winrt::Microsoft::ReactNative::IViewComponentDescriptor GetDescriptor(
    std::shared_ptr<std::string const> &flavor) const noexcept;
winrt::Microsoft::ReactNative::IViewComponentDescriptor GetDescriptor(
    facebook::react::ComponentHandle handle) const noexcept;

static void AddToProperties(const winrt::Microsoft::ReactNative::ReactPropertyBag &props, std::shared_ptr<WindowsComponentDescriptorRegistry>& registry) noexcept;
static std::shared_ptr<WindowsComponentDescriptorRegistry> FromProperties(const winrt::Microsoft::ReactNative::ReactPropertyBag &props) noexcept;

private: 
  
std::map<std::shared_ptr<const std::string>, winrt::Microsoft::ReactNative::IViewComponentDescriptor const> m_userDescriptorsByName;
std::map<facebook::react::ComponentHandle, winrt::Microsoft::ReactNative::IViewComponentDescriptor const> m_userDescriptorsByHandle;
 std::vector<std::shared_ptr<const std::string>> m_descriptorFlavors;
 std::shared_ptr<facebook::react::ComponentDescriptorProviderRegistry> m_componentDescriptorRegistry;
};

} // namespace Microsoft::ReactNative
