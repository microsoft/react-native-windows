#include "pch.h"

#include "App.h"

#include "AutolinkedNativeModules.g.h"
#include "ReactPackageProvider.h"

#include <winrt/InteropTestModuleCS.h>

using namespace winrt::integrationtest;
using namespace winrt::integrationtest::implementation;
using namespace winrt;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::ApplicationModel;

/// <summary>
/// Initializes the singleton application object.  This is the first line of
/// authored code executed, and as such is the logical equivalent of main() or
/// WinMain().
/// </summary>
App::App() noexcept {
#if BUNDLE
  JavaScriptBundleFile(L"index.windows");
  InstanceSettings().UseFastRefresh(false);
#else
  JavaScriptBundleFile(L"index");
  InstanceSettings().UseFastRefresh(true);
#endif

#if _DEBUG
  InstanceSettings().UseDeveloperSupport(true);
#else
  InstanceSettings().UseDeveloperSupport(false);
#endif

  RegisterAutolinkedNativeModulePackages(PackageProviders()); // Includes any autolinked modules

  PackageProviders().Append(make<ReactPackageProvider>()); // Includes all modules in this project
  PackageProviders().Append(winrt::InteropTestModuleCS::ReactPackageProvider());

  m_harness = winrt::make_self<IntegrationTest::TestHostHarness>(Host());

  InitializeComponent();
}

/// <summary>
/// Invoked when the application is launched normally by the end user.  Other entry points
/// will be used such as when the application is launched to open a specific file.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void App::OnLaunched(activation::LaunchActivatedEventArgs const &e) {
  super::OnLaunched(e);

  Frame rootFrame = Window::Current().Content().as<Frame>();
  rootFrame.Navigate(xaml_typename<integrationtest::MainPage>(), box_value(e.Arguments()));
}

/// <summary>
/// Invoked when application execution is being suspended.  Application state is saved
/// without knowing whether the application will be terminated or resumed with the contents
/// of memory still intact.
/// </summary>
/// <param name="sender">The source of the suspend request.</param>
/// <param name="e">Details about the suspend request.</param>
void App::OnSuspending([[maybe_unused]] IInspectable const &sender, [[maybe_unused]] SuspendingEventArgs const &e) {
  // Save application state and stop any background activity
}

/// <summary>
/// Invoked when Navigation to a certain page fails
/// </summary>
/// <param name="sender">The Frame which failed navigation</param>
/// <param name="e">Details about the navigation failure</param>
void App::OnNavigationFailed(IInspectable const &, NavigationFailedEventArgs const &e) {
  throw hresult_error(E_FAIL, hstring(L"Failed to load Page ") + e.SourcePageType().Name);
}

/// <summary>
/// Returns the test harness driving the application
/// </summary>
IntegrationTest::TestHostHarness &App::TestHarness() {
  return *m_harness;
}
