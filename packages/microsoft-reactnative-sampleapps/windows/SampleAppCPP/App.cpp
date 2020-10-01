// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "App.h"

#include "AutolinkedNativeModules.g.h"
#include <CppWinRTIncludes.h>
#include <ReactPackageProvider.h>
#include <ReactPropertyBag.h>
#include <winrt/SampleLibraryCS.h>
#include <winrt/SampleLibraryCpp.h>

using namespace winrt;
using namespace xaml;
using namespace xaml::Controls;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;

namespace winrt::SampleAppCpp::implementation {

/// <summary>
/// Initializes the singleton application object.  This is the first line of
/// authored code executed, and as such is the logical equivalent of main() or
/// WinMain().
/// </summary>
App::App() noexcept {
#if BUNDLE
  JavaScriptBundleFile(L"index.windows");
  InstanceSettings().UseWebDebugger(false);
  InstanceSettings().UseFastRefresh(false);
#else
  JavaScriptBundleFile(L"index");
  InstanceSettings().UseWebDebugger(true);
  InstanceSettings().UseFastRefresh(true);
#endif

#if _DEBUG
  InstanceSettings().UseDeveloperSupport(true);
#else
  InstanceSettings().UseDeveloperSupport(false);
#endif

  ReactPropertyBag::Set(InstanceSettings().Properties(), ReactPropertyId<int>{L"Prop1"}, 42);
  ReactPropertyBag::Set(InstanceSettings().Properties(), ReactPropertyId<hstring>{L"Prop2"}, L"Hello World!");

  RegisterAutolinkedNativeModulePackages(PackageProviders()); // Includes any autolinked modules

  PackageProviders().Append(make<ReactPackageProvider>()); // Includes all modules in this project
  PackageProviders().Append(winrt::SampleLibraryCpp::ReactPackageProvider());
  PackageProviders().Append(winrt::SampleLibraryCS::ReactPackageProvider());

  InitializeComponent();
}

/// <summary>
/// Invoked when the application is launched normally by the end user.  Other entry points
/// will be used such as when the application is launched to open a specific file.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void App::OnLaunched(LaunchActivatedEventArgs const &e) {
  super::OnLaunched(e);
  Frame rootFrame = Window::Current().Content().as<Frame>();
  rootFrame.Navigate(xaml_typename<MainPage>(), box_value(e.Arguments()));
}

} // namespace winrt::SampleAppCpp::implementation
