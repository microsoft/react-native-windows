#include "pch.h"

#include "App.h"
#include "ReactPackageProvider.h"

// clang-format off

using namespace winrt::<%=ns%>;
using namespace winrt::<%=ns%>::implementation;

/// <summary>
/// Initializes the singleton application object.  This is the first line of
/// authored code executed, and as such is the logical equivalent of main() or
/// WinMain().
/// </summary>
App::App() noexcept
{
    MainComponentName(L"<%=name%>");

#if BUNDLE
    JavaScriptBundleFile(L"index.windows");
    InstanceSettings().UseWebDebugger(false);
    InstanceSettings().UseLiveReload(false);
#else
    JavaScriptMainModuleName(L"index");
    InstanceSettings().UseWebDebugger(true);
    InstanceSettings().UseLiveReload(true);
#endif

#if _DEBUG
    InstanceSettings().EnableDeveloperMenu(true);
#else
    InstanceSettings().EnableDeveloperMenu(false);
#endif

    PackageProviders().Append(make<ReactPackageProvider>()); // Includes all modules in this project

    InitializeComponent();

    // This works around a cpp/winrt bug with composable/aggregable types tracked
    // by 22116519
    AddRef();
    m_inner.as<::IUnknown>()->Release();
}

// clang-format on
