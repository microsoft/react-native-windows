// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactApplication.h"
#include "ReactApplication.g.cpp"
#include "ReactNativeHost.h"

using namespace winrt;
using namespace Microsoft::ReactNative;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::Microsoft::ReactNative::implementation {
ReactApplication::ReactApplication()
    : m_delegate(CreateReactApplicationDelegate()) {
  Suspending({this, &ReactApplication::OnSuspending});

#if defined _DEBUG && \
    !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
  UnhandledException(
      [this](IInspectable const &, UnhandledExceptionEventArgs const &e) {
        if (IsDebuggerPresent()) {
          auto errorMessage = e.Message();
          __debugbreak();
        }
      });
#endif
}

Microsoft::ReactNative::ReactNativeHost ReactApplication::Host() {
  if (m_host == nullptr) {
    m_host = overridable().HostCore();
  }

  return m_host;
}

Microsoft::ReactNative::ReactNativeHost ReactApplication::HostCore() {
  throw hresult_not_implemented(
      L"You must implement ReactApplication.HostCore.");
}

void ReactApplication::OnLaunched(LaunchActivatedEventArgs const &e) {
  __super::OnLaunched(e);
  // auto args = std::wstring(e.Arguments().c_str());
  this->OnCreate(e);
}

/// <summary>
/// Invoked when the application is launched normally by the end user.  Other
/// entry points will be used such as when the application is launched to open a
/// specific file.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void ReactApplication::OnCreate(LaunchActivatedEventArgs const &e) {
#if defined _DEBUG
  if (IsDebuggerPresent()) {
    this->DebugSettings().EnableFrameRateCounter(TRUE);

    SystemNavigationManager::GetForCurrentView().AppViewBackButtonVisibility(
        AppViewBackButtonVisibility::Visible);
  }
#endif

  Frame rootFrame{nullptr};
  auto content = Window::Current().Content();
  if (content) {
    rootFrame = content.try_as<Frame>();
  }

  // Do not repeat app initialization when the Window already has content,
  // just ensure that the window is active
  if (rootFrame == nullptr) {
    // Create a Frame to act as the navigation context and associate it with
    // a SuspensionManager key
    rootFrame = Frame();

    rootFrame.NavigationFailed({this, &ReactApplication::OnNavigationFailed});

    if (e.PreviousExecutionState() == ApplicationExecutionState::Terminated) {
      // Restore the saved session state only when appropriate, scheduling the
      // final launch steps after the restore is complete
    }

    if (e.PrelaunchActivated() == false) {
      if (rootFrame.Content() == nullptr) {
        // When the navigation stack isn't restored navigate to the first page,
        // configuring the new page by passing required information as a
        // navigation parameter
        auto args = e.Arguments();
        content = m_delegate.OnCreate(args);
        rootFrame.Content(content);
      }

      // Place the frame in the current Window
      Window::Current().Content(rootFrame);
      // Ensure the current window is active
      Window::Current().Activate();
    }
  } else {
    if (e.PrelaunchActivated() == false) {
      if (rootFrame.Content() == nullptr) {
        // When the navigation stack isn't restored navigate to the first page,
        // configuring the new page by passing required information as a
        // navigation parameter
        auto args = e.Arguments();
        content = m_delegate.OnCreate(args);
        rootFrame.Content(content);
      }
      // Ensure the current window is active
      Window::Current().Activate();
    }
  }
}

/// <summary>
/// Invoked when application execution is being suspended.  Application state is
/// saved without knowing whether the application will be terminated or resumed
/// with the contents of memory still intact.
/// </summary>
/// <param name="sender">The source of the suspend request.</param>
/// <param name="e">Details about the suspend request.</param>
void ReactApplication::OnSuspending(
    [[maybe_unused]] IInspectable const &sender,
    [[maybe_unused]] Windows::ApplicationModel::SuspendingEventArgs const &e) {
  // Save application state and stop any background activity
}

/// <summary>
/// Invoked when Navigation to a certain page fails
/// </summary>
/// <param name="sender">The Frame which failed navigation</param>
/// <param name="e">Details about the navigation failure</param>
void ReactApplication::OnNavigationFailed(
    IInspectable const &,
    NavigationFailedEventArgs const &e) {
  throw hresult_error(
      E_FAIL, hstring(L"Failed to load Page ") + e.SourcePageType().Name);
}
} // namespace winrt::Microsoft::ReactNative::implementation
