#include "pch.h"
#include "CoreApp.h"
#if __has_include("CoreApp.g.cpp")
#include "CoreApp.g.cpp"
#endif

#include "CppWinRTIncludes.h"

#ifndef USE_WINUI3
#include <winrt/Windows.UI.Xaml.Navigation.h>
#endif

namespace winrt::Microsoft::ReactNative::implementation
{
/// <summary>
/// Invoked when the application is launched normally by the end user.  Other entry points
/// will be used such as when the application is launched to open a specific file.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void CoreApp::OnLaunched(activation::LaunchActivatedEventArgs const &e) {
  //TODO
  //super::OnLaunched(e);

  auto rootFrame = xaml::Window::Current().Content().as<xaml::Controls::Frame>();
  rootFrame.Navigate(xaml_typename<winrt::Microsoft::ReactNative::CoreAppPage>(), box_value(e.Arguments()));
}

/// <summary>
/// Invoked when the application is activated by some means other than normal launching.
/// </summary>
void CoreApp::OnActivated(activation::IActivatedEventArgs const &e) {
  auto preActivationContent = xaml::Window::Current().Content();
  //TODO
  //super::OnActivated(e);
  if (!preActivationContent && xaml::Window::Current()) {
    auto rootFrame = xaml::Window::Current().Content().as<xaml::Controls::Frame>();
    rootFrame.Navigate(xaml_typename<winrt::Microsoft::ReactNative::CoreAppPage>(), nullptr);
  }
}

/// <summary>
/// Invoked when application execution is being suspended.  Application state is saved
/// without knowing whether the application will be terminated or resumed with the contents
/// of memory still intact.
/// </summary>
/// <param name="sender">The source of the suspend request.</param>
/// <param name="e">Details about the suspend request.</param>
void CoreApp::OnSuspending(
    [[maybe_unused]] IInspectable const &sender,
    [[maybe_unused]] Windows::ApplicationModel::SuspendingEventArgs const &e) {
  // Save application state and stop any background activity
}

/// <summary>
/// Invoked when Navigation to a certain page fails
/// </summary>
/// <param name="sender">The Frame which failed navigation</param>
/// <param name="e">Details about the navigation failure</param>
void CoreApp::OnNavigationFailed(
    IInspectable const &,
    xaml::Navigation::NavigationFailedEventArgs const &e) {
  throw hresult_error(E_FAIL, hstring(L"Failed to load Page ") + e.SourcePageType().Name);
}



}
