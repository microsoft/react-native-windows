#pragma once
#include "Xaml.XamlApplication.g.h"

#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>
#include <winrt/Windows.Foundation.h>

#include "winrt/Microsoft.UI.Xaml.Hosting.h"
#include "winrt/Microsoft.UI.Xaml.Interop.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.h"

#include "winrt/Windows.UI.Xaml.Interop.h"

#include "XamlMetaDataProvider.h"

namespace winrt::Microsoft::ReactNative::Xaml::implementation {
struct XamlApplication : XamlApplicationT<XamlApplication> {
  XamlApplication();
  ~XamlApplication();

  static void EnsureCreated() {
    if (Current() == nullptr) {
      s_current = winrt::make<winrt::Microsoft::ReactNative::Xaml::implementation::XamlApplication>();
    }
  }

  static winrt::Microsoft::ReactNative::Xaml::XamlApplication Current() {
    return s_current;
  }

  void AddMetadataProvider(winrt::Microsoft::UI::Xaml::Markup::IXamlMetadataProvider const &otherProvider);
  winrt::Microsoft::UI::Xaml::Markup::IXamlType GetXamlType(winrt::Windows::UI::Xaml::Interop::TypeName const &type);
  winrt::Microsoft::UI::Xaml::Markup::IXamlType GetXamlType(hstring const &fullName);
  com_array<winrt::Microsoft::UI::Xaml::Markup::XmlnsDefinition> GetXmlnsDefinitions();

 private:
  static winrt::Microsoft::ReactNative::Xaml::XamlApplication s_current;
  std::vector<winrt::Microsoft::UI::Xaml::Markup::IXamlMetadataProvider> m_providers;
};
} // namespace winrt::Microsoft::ReactNative::Xaml::implementation
namespace winrt::Microsoft::ReactNative::Xaml::factory_implementation {
struct XamlApplication : XamlApplicationT<XamlApplication, implementation::XamlApplication> {};
} // namespace winrt::Microsoft::ReactNative::Xaml::factory_implementation
