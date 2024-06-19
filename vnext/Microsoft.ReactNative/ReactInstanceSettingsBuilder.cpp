#include "pch.h"
#include "ReactInstanceSettings.h"
#include "ReactInstanceSettingsBuilder.h"
#include "ReactInstanceSettingsBuilder.g.cpp"

namespace winrt::ReactNative
{
    using namespace winrt::Microsoft::ReactNative;
}

namespace winrt::UI
{
    using namespace winrt::Microsoft::UI;
}

namespace winrt::Microsoft::ReactNative::implementation
{
    ReactInstanceSettingsBuilder::ReactInstanceSettingsBuilder()
    {
        m_reactInstanceSettings = winrt::make<winrt::ReactNative::implementation::ReactInstanceSettings>();

#if _DEBUG
        m_reactInstanceSettings.UseDirectDebugger(true);
        m_reactInstanceSettings.UseDeveloperSupport(true);
#else
        m_reactInstanceSettings.UseDirectDebugger(false);
        m_reactInstanceSettings.UseDeveloperSupport(false);
#endif
    }

    winrt::ReactNative::ReactInstanceSettings ReactInstanceSettingsBuilder::ReactInstanceSettings()
    {
        return m_reactInstanceSettings;
    }

    winrt::ReactNative::ReactInstanceSettingsBuilder ReactInstanceSettingsBuilder::SetBundleRootPath(hstring const& path)
    {
#if BUNDLE
        m_reactInstanceSettings.BundleRootPath(path.c_str());
#endif
        return *this;
    }

    winrt::ReactNative::ReactInstanceSettingsBuilder ReactInstanceSettingsBuilder::SetJavaScriptBundleFile(hstring const& file)
    {
        m_reactInstanceSettings.JavaScriptBundleFile(file.c_str());

        return *this;
    }

    winrt::ReactNative::ReactInstanceSettingsBuilder ReactInstanceSettingsBuilder::SetUseFastRefresh(
        bool const &state)
    {
        m_reactInstanceSettings.UseFastRefresh(state);

        return *this;
    }
}
