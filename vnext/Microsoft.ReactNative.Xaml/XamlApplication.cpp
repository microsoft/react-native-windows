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
  auto resourceManager = winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager(L"Microsoft.UI.Xaml.Controls.pri");

  this->ResourceManagerRequested([resourceManager](auto&&, ResourceManagerRequestedEventArgs args) {
      args.CustomResourceManager(resourceManager);
      });
  winrt::Microsoft::UI::Xaml::Hosting::WindowsXamlManager::InitializeForCurrentThread();

  auto winUIResources = winrt::Microsoft::UI::Xaml::Controls::XamlControlsResources();
  Resources().MergedDictionaries().Append(winUIResources);
}

XamlApplication::~XamlApplication() {
  s_current = nullptr;
}

IXamlType XamlApplication::GetXamlType(TypeName const &type) {
  return AppProvider()->GetXamlType(type);
}

IXamlType XamlApplication::GetXamlType(::winrt::hstring const &fullName) {
  return AppProvider()->GetXamlType(fullName);
}

::winrt::com_array<::winrt::Microsoft::UI::Xaml::Markup::XmlnsDefinition> XamlApplication::GetXmlnsDefinitions() {
  return AppProvider()->GetXmlnsDefinitions();
}

void XamlApplication::AddMetadataProvider(
    winrt::Microsoft::UI::Xaml::Markup::IXamlMetadataProvider const &otherProvider) {
  AppProvider()->AddProvider(otherProvider);
}

winrt::Microsoft::ReactNative::Xaml::XamlApplication XamlApplication::s_current{nullptr};

} // namespace winrt::Microsoft::ReactNative::Xaml::implementation
