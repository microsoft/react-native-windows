#include "pch.h"
#include "XamlApplication.h"
#include "XamlApplication.g.cpp"

namespace winrt::Microsoft::ReactNative::implementation
{
    void XamlApplication::EnsureCreated()
    {
        throw hresult_not_implemented();
    }
    winrt::Microsoft::ReactNative::XamlApplication XamlApplication::Current()
    {
        throw hresult_not_implemented();
    }
    void XamlApplication::AddMetadataProvider(winrt::Microsoft::UI::Xaml::Markup::IXamlMetadataProvider const& otherProvider)
    {
        throw hresult_not_implemented();
    }
    winrt::Microsoft::UI::Xaml::Markup::IXamlType XamlApplication::GetXamlType(winrt::Windows::UI::Xaml::Interop::TypeName const& type)
    {
        throw hresult_not_implemented();
    }
    winrt::Microsoft::UI::Xaml::Markup::IXamlType XamlApplication::GetXamlType(hstring const& fullName)
    {
        throw hresult_not_implemented();
    }
    com_array<winrt::Microsoft::UI::Xaml::Markup::XmlnsDefinition> XamlApplication::GetXmlnsDefinitions()
    {
        throw hresult_not_implemented();
    }
}
