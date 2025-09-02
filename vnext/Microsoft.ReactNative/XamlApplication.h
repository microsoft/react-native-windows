#pragma once
#include "XamlApplication.g.h"

#include "winrt/Microsoft.UI.Xaml.Hosting.h"
#include "winrt/Microsoft.UI.Xaml.Interop.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.h"

#include "winrt/Windows.UI.Xaml.Interop.h"

#include "XamlMetaDataProvider.h"

namespace winrt::Microsoft::ReactNative::implementation
{
    struct XamlApplication : XamlApplicationT<XamlApplication>
    {
      XamlApplication();
      ~XamlApplication();

      static void EnsureCreated()
      {
        if (Current() == nullptr) {
          s_current = winrt::make<winrt::Microsoft::ReactNative::implementation::XamlApplication>();
        }
      }

      static winrt::Microsoft::ReactNative::XamlApplication Current()
      {
        return s_current;
      }

      void AddMetadataProvider(winrt::Microsoft::UI::Xaml::Markup::IXamlMetadataProvider const& otherProvider);
      winrt::Microsoft::UI::Xaml::Markup::IXamlType GetXamlType(winrt::Windows::UI::Xaml::Interop::TypeName const& type);
      winrt::Microsoft::UI::Xaml::Markup::IXamlType GetXamlType(hstring const& fullName);
      com_array<winrt::Microsoft::UI::Xaml::Markup::XmlnsDefinition> GetXmlnsDefinitions();

      private:
      static winrt::Microsoft::ReactNative::XamlApplication s_current;
      std::vector<winrt::Microsoft::UI::Xaml::Markup::IXamlMetadataProvider> m_providers;
    };
}
namespace winrt::Microsoft::ReactNative::factory_implementation
{
    struct XamlApplication : XamlApplicationT<XamlApplication, implementation::XamlApplication>
    {
    };
}
