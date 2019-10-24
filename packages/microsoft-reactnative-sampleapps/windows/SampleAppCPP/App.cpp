// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "App.h"
#include "ReactPackageProvider.h"
#include "winrt/SampleLibraryCPP.h"
#include "winrt/SampleLibraryCS.h"

namespace winrt::SampleApp::implementation {

/// <summary>
/// Initializes the singleton application object.  This is the first line of
/// authored code executed, and as such is the logical equivalent of main() or
/// WinMain().
/// </summary>
App::App() noexcept {
  MainComponentName(L"SampleApp");
  JavaScriptMainModuleName(L"index.windows");

  PackageProviders().Append(
      make<ReactPackageProvider>()); // Includes all modules in this project
  PackageProviders().Append(winrt::SampleLibraryCPP::ReactPackageProvider());
  PackageProviders().Append(winrt::SampleLibraryCS::ReactPackageProvider());

  InitializeComponent();

  // This works around a cpp/winrt bug with composable/aggregable types tracked
  // by 22116519
  AddRef();
  m_inner.as<::IUnknown>()->Release();
}

} // namespace winrt::SampleApp::implementation
