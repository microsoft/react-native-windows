#include "pch.h"

#include "XamlApplication.h"
#include "XamlApplication.g.cpp"

#include "winrt/Microsoft.UI.Xaml.XamlTypeInfo.h"

namespace winrt::ReactNativeXamlFabric::implementation
{
    using namespace ::winrt::Microsoft::UI::Xaml::Markup;
    using namespace ::winrt::Windows::UI::Xaml::Interop;

    XamlApplication::XamlApplication()
    {
      s_current = *this;
      winrt::Microsoft::UI::Xaml::Hosting::WindowsXamlManager::InitializeForCurrentThread();

      auto winUIResources = winrt::Microsoft::UI::Xaml::Controls::XamlControlsResources();
      Resources().MergedDictionaries().Append(winUIResources);
    }

    XamlApplication::~XamlApplication()
    {
      s_current = nullptr;
    }

    IXamlType XamlApplication::GetXamlType(TypeName const& type)
    {
        return AppProvider()->GetXamlType(type);
    }

    IXamlType XamlApplication::GetXamlType(::winrt::hstring const& fullName)
    {
        return AppProvider()->GetXamlType(fullName);
    }

    ::winrt::com_array<::winrt::Microsoft::UI::Xaml::Markup::XmlnsDefinition> XamlApplication::GetXmlnsDefinitions()
    {
        return AppProvider()->GetXmlnsDefinitions();
    }

    void XamlApplication::AddMetadataProvider(winrt::Microsoft::UI::Xaml::Markup::IXamlMetadataProvider const& otherProvider)
    {
        AppProvider()->AddProvider(otherProvider);
    }

    winrt::ReactNativeXamlFabric::XamlApplication XamlApplication::s_current{ nullptr };

}
