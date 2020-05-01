// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "App.h"
#include "ReactPackageProvider.h"
#include "winrt/SampleLibraryCS.h"
#include "winrt/SampleLibraryCpp.h"

namespace winrt::SampleAppCpp::implementation {

/// <summary>
/// Initializes the singleton application object.  This is the first line of
/// authored code executed, and as such is the logical equivalent of main() or
/// WinMain().
/// </summary>
App::App() noexcept {
  MainComponentName(L"SampleApp");

#if BUNDLE
  JavaScriptBundleFile(L"index.windows");
  InstanceSettings().UseWebDebugger(false);
  InstanceSettings().UseFastRefresh(false);
#else
  JavaScriptMainModuleName(L"index");
  InstanceSettings().UseWebDebugger(true);
  InstanceSettings().UseFastRefresh(true);
#endif

#if _DEBUG
  InstanceSettings().EnableDeveloperMenu(true);
#else
  InstanceSettings().EnableDeveloperMenu(false);
#endif

  PackageProviders().Append(make<ReactPackageProvider>()); // Includes all modules in this project
  PackageProviders().Append(winrt::SampleLibraryCpp::ReactPackageProvider());
  PackageProviders().Append(winrt::SampleLibraryCS::ReactPackageProvider());

  InitializeComponent();
}

} // namespace winrt::SampleAppCpp::implementation
