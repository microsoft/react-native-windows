// WARNING: Please don't edit this file. It was generated by C++/WinRT v2.0.240405.15

#pragma once
#include "winrt/Microsoft.ReactNative.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
namespace winrt::Microsoft::ReactNative::Xaml::implementation
{
    template <typename D, typename... I>
    struct WINRT_IMPL_EMPTY_BASES XamlMetaDataProvider_base : implements<D, Microsoft::ReactNative::Xaml::XamlMetaDataProvider, I...>
    {
        using base_type = XamlMetaDataProvider_base;
        using class_type = Microsoft::ReactNative::Xaml::XamlMetaDataProvider;
        using implements_type = typename XamlMetaDataProvider_base::implements_type;
        using implements_type::implements_type;
        
        hstring GetRuntimeClassName() const
        {
            return L"Microsoft.ReactNative.Xaml.XamlMetaDataProvider";
        }
    };
}
namespace winrt::Microsoft::ReactNative::Xaml::factory_implementation
{
    template <typename D, typename T, typename... I>
    struct WINRT_IMPL_EMPTY_BASES XamlMetaDataProviderT : implements<D, winrt::Windows::Foundation::IActivationFactory, I...>
    {
        using instance_type = Microsoft::ReactNative::Xaml::XamlMetaDataProvider;

        hstring GetRuntimeClassName() const
        {
            return L"Microsoft.ReactNative.Xaml.XamlMetaDataProvider";
        }
        auto ActivateInstance() const
        {
            return make<T>();
        }
    };
}

#if defined(WINRT_FORCE_INCLUDE_XAMLMETADATAPROVIDER_XAML_G_H) || __has_include("XamlMetaDataProvider.xaml.g.h")

#include "XamlMetaDataProvider.xaml.g.h"

#else

namespace winrt::Microsoft::ReactNative::Xaml::implementation
{
    template <typename D, typename... I>
    using XamlMetaDataProviderT = XamlMetaDataProvider_base<D, I...>;
}

#endif
