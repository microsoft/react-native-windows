#pragma once
#include "XamlApplication.g.h"

#include "winrt/Microsoft.UI.Xaml.Hosting.h"
#include "winrt/Microsoft.UI.Xaml.Interop.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.h"

#include "winrt/Windows.UI.Xaml.Interop.h"

#include "XamlMetaDataProvider.h"

namespace winrt::Microsoft::ReactNative::Xaml::implementation {
struct XamlApplication : XamlApplicationT<XamlApplication> {
  using IXamlType = ::winrt::Microsoft::UI::Xaml::Markup::IXamlType;

  XamlApplication();
  ~XamlApplication();

  static void EnsureCreated() {
    if (Current() == nullptr) {
      // Create this type
      winrt::make<winrt::Microsoft::ReactNative::Xaml::implementation::XamlApplication>();
    }
  }

  static winrt::Microsoft::ReactNative::Xaml::XamlApplication Current() {
    return s_current;
  }

  IXamlType GetXamlType(::winrt::Windows::UI::Xaml::Interop::TypeName const &type);

  IXamlType GetXamlType(::winrt::hstring const &fullName);

  ::winrt::com_array<::winrt::Microsoft::UI::Xaml::Markup::XmlnsDefinition> GetXmlnsDefinitions();

  void AddMetadataProvider(winrt::Microsoft::UI::Xaml::Markup::IXamlMetadataProvider const &otherProvider);

 private:
  static winrt::Microsoft::ReactNative::Xaml::XamlApplication s_current;
  std::vector<winrt::Microsoft::UI::Xaml::Markup::IXamlMetadataProvider> m_providers;
};
} // namespace winrt::Microsoft::ReactNative::Xaml::implementation
namespace winrt::Microsoft::ReactNative::Xaml::factory_implementation {
struct XamlApplication : XamlApplicationT<XamlApplication, implementation::XamlApplication> {};
} // namespace winrt::Microsoft::ReactNative::Xaml::factory_implementation
