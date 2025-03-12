#pragma once
#include "XamlApplication.g.h"

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Hosting.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Interop.h"

#include "winrt/Windows.UI.Xaml.Interop.h"

#include "XamlTypeInfo.xaml.g.h"
#include "XamlMetaDataProvider.h"

namespace winrt::ReactNativeXamlFabric::implementation {
struct XamlApplication : XamlApplicationT<XamlApplication> {
  using IXamlType = ::winrt::Microsoft::UI::Xaml::Markup::IXamlType;

  XamlApplication();
  ~XamlApplication();

  static void EnsureCreated() {
    if (Current() == nullptr) {
      // Create this type
      winrt::make<winrt::ReactNativeXamlFabric::implementation::XamlApplication>();
    }
  }

  static winrt::ReactNativeXamlFabric::XamlApplication Current() {
    return s_current;
  }

  IXamlType GetXamlType(::winrt::Windows::UI::Xaml::Interop::TypeName const &type);

  IXamlType GetXamlType(::winrt::hstring const &fullName);

  ::winrt::com_array<::winrt::Microsoft::UI::Xaml::Markup::XmlnsDefinition> GetXmlnsDefinitions();

  void AddMetadataProvider(winrt::Microsoft::UI::Xaml::Markup::IXamlMetadataProvider const &otherProvider);

 private:
  static winrt::ReactNativeXamlFabric::XamlApplication s_current;
  winrt::com_ptr<XamlMetaDataProvider> _appProvider;
  winrt::com_ptr<XamlMetaDataProvider> AppProvider() {
    if (!_appProvider) {
      _appProvider = winrt::make_self<XamlMetaDataProvider>();
    }
    return _appProvider;
  }
};
} // namespace winrt::ReactNativeXamlFabric::implementation
namespace winrt::ReactNativeXamlFabric::factory_implementation {
struct XamlApplication : XamlApplicationT<XamlApplication, implementation::XamlApplication> {};
} // namespace winrt::ReactNativeXamlFabric::factory_implementation
