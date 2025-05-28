#include "pch.h"

#include "XamlApplication.h"

#include "XamlApplication.g.cpp"

#include "winrt/Microsoft.UI.Xaml.XamlTypeInfo.h"

namespace winrt::Microsoft::ReactNative::Xaml::implementation {

using namespace ::winrt::Microsoft::UI::Xaml;
using namespace ::winrt::Microsoft::UI::Xaml::Markup;
using namespace ::winrt::Windows::UI::Xaml::Interop;

XamlApplication::XamlApplication() {
  s_current = *this;

  // TODO: It's probably not a good idea to only load the controls pri file, there are other ones too.
  auto resourceManager =
      winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager(L"Microsoft.UI.Xaml.Controls.pri");

  this->ResourceManagerRequested([resourceManager](auto &&, ResourceManagerRequestedEventArgs args) {
    args.CustomResourceManager(resourceManager);
  });
  winrt::Microsoft::UI::Xaml::Hosting::WindowsXamlManager::InitializeForCurrentThread();

  m_providers.push_back(
      winrt::make_self<XamlMetaDataProvider>()
          .as<winrt::Microsoft::UI::Xaml::Markup::IXamlMetadataProvider>()); // Default generated provider
  m_providers.push_back(winrt::Microsoft::UI::Xaml::XamlTypeInfo::XamlControlsXamlMetaDataProvider());

  auto winUIResources = winrt::Microsoft::UI::Xaml::Controls::XamlControlsResources();
  Resources().MergedDictionaries().Append(winUIResources);
}

XamlApplication::~XamlApplication() {
  s_current = nullptr;
}

IXamlType XamlApplication::GetXamlType(TypeName const &type) {
  for (auto &&provider : m_providers) {
    if (auto result = provider.GetXamlType(type)) {
      return result;
    }
  }
  return nullptr;
}

IXamlType XamlApplication::GetXamlType(::winrt::hstring const &fullName) {
  for (auto &&provider : m_providers) {
    if (auto result = provider.GetXamlType(fullName)) {
      return result;
    }
  }

  return nullptr;
}

::winrt::com_array<::winrt::Microsoft::UI::Xaml::Markup::XmlnsDefinition> XamlApplication::GetXmlnsDefinitions() {
  std::vector<::winrt::Microsoft::UI::Xaml::Markup::XmlnsDefinition> allDefinitions;
  for (const auto &provider : m_providers) {
    auto definitionsCurrentProvider = provider.GetXmlnsDefinitions();
    for (const auto &definition : definitionsCurrentProvider) {
      allDefinitions.insert(allDefinitions.begin(), definition);
    }
  }
  return winrt::com_array<::winrt::Microsoft::UI::Xaml::Markup::XmlnsDefinition>(
      allDefinitions.begin(), allDefinitions.end());
}

void XamlApplication::AddMetadataProvider(
    winrt::Microsoft::UI::Xaml::Markup::IXamlMetadataProvider const &otherProvider) {
  m_providers.push_back(otherProvider);
}

winrt::Microsoft::ReactNative::Xaml::XamlApplication XamlApplication::s_current{nullptr};

} // namespace winrt::Microsoft::ReactNative::Xaml::implementation
