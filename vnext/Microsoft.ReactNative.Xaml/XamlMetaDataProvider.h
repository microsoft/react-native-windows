﻿//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------
#pragma once

#include <unknwn.h>
#include <functional>
#include <map>
#include <vector>

// Undefine GetCurrentTime macro to prevent
// conflict with Storyboard::GetCurrentTime
#undef GetCurrentTime

#include "XamlMetaDataProvider.g.h"
#include "XamlTypeInfo.xaml.g.h"

/*
    If the file above was not generated, you may be missing a declaration
    for the XamlMetaDataProvider runtimeclass in your IDL.

    namespace Microsoft.ReactNative.Xaml
    {
        runtimeclass XamlMetaDataProvider : Microsoft.UI.Xaml.Markup.IXamlMetadataProvider
        {
            XamlMetaDataProvider();
        };
    }
*/

namespace winrt::Microsoft::ReactNative::Xaml::implementation {
using IXamlMember = ::winrt::Microsoft::UI::Xaml::Markup::IXamlMember;
using IXamlType = ::winrt::Microsoft::UI::Xaml::Markup::IXamlType;

struct XamlMetaDataProvider
    : public ::winrt::Microsoft::ReactNative::Xaml::implementation::XamlMetaDataProvider_base<XamlMetaDataProvider> {
  IXamlType GetXamlType(::winrt::Windows::UI::Xaml::Interop::TypeName const &type);
  IXamlType GetXamlType(::winrt::hstring const &fullName);
  ::winrt::com_array<::winrt::Microsoft::UI::Xaml::Markup::XmlnsDefinition> GetXmlnsDefinitions();

  void AddProvider(::winrt::Microsoft::UI::Xaml::Markup::IXamlMetadataProvider const &otherProvider) {
    Provider()->AddProvider(otherProvider);
  }

 private:
  std::shared_ptr<XamlTypeInfoProvider> _provider;
  std::shared_ptr<XamlTypeInfoProvider> Provider();
};
} // namespace winrt::Microsoft::ReactNative::Xaml::implementation

namespace winrt::Microsoft::ReactNative::Xaml::factory_implementation {
struct XamlMetaDataProvider : XamlMetaDataProviderT<XamlMetaDataProvider, implementation::XamlMetaDataProvider> {};
} // namespace winrt::Microsoft::ReactNative::Xaml::factory_implementation
