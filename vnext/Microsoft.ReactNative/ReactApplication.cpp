// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactApplication.h"
#include "ReactApplication.g.cpp"

#include "IReactDispatcher.h"
#include "Modules/LinkingManagerModule.h"
#include "ReactNativeHost.h"

#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Navigation.h>

using namespace winrt;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::Microsoft::ReactNative::implementation {

ReactApplication::ReactApplication() = default;

ReactApplication::ReactApplication(IInspectable const &outer) noexcept : ReactApplication{} {
  // The factory is usually called in the base generated class. We call it here to pass correct
  // 'outer' interface to enable inheritance from the ReactApplication class in user code.
  impl::call_factory<Windows::UI::Xaml::Application, Windows::UI::Xaml::IApplicationFactory>(
      [&](Windows::UI::Xaml::IApplicationFactory const &f) {
        [[maybe_unused]] auto winrt_impl_discarded =
            f.CreateInstance(outer ? outer : static_cast<IInspectable const &>(*this), this->m_inner);
      });

  Suspending({this, &ReactApplication::OnSuspending});

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
  UnhandledException([this](IInspectable const &, UnhandledExceptionEventArgs const &e) {
    if (IsDebuggerPresent()) {
      auto errorMessage = e.Message();
      __debugbreak();
    }
  });
#endif
}

ReactNative::ReactInstanceSettings ReactApplication::InstanceSettings() noexcept {
  if (!m_instanceSettings) {
    m_instanceSettings = make<ReactInstanceSettings>();
    ReactDispatcher::SetUIThreadDispatcher(m_instanceSettings.Properties());
  }

  return m_instanceSettings;
}

void ReactApplication::InstanceSettings(ReactNative::ReactInstanceSettings const &value) noexcept {
  m_instanceSettings = value;
}

IVector<IReactPackageProvider> ReactApplication::PackageProviders() noexcept {
  if (!m_packageProviders) {
    m_packageProviders = single_threaded_vector<IReactPackageProvider>();
  }

  return m_packageProviders;
}

void ReactApplication::PackageProviders(
    Windows::Foundation::Collections::IVector<IReactPackageProvider> const &value) noexcept {
  m_packageProviders = value;
}

ReactNative::ReactNativeHost ReactApplication::Host() noexcept {
  if (!m_host) {
    m_host = make<ReactNativeHost>();
    m_host.InstanceSettings(InstanceSettings());
    m_host.PackageProviders(PackageProviders());
  }

  return m_host;
}

hstring ReactApplication::MainComponentName() noexcept {
  return InstanceSettings().MainComponentName();
}

void ReactApplication::MainComponentName(hstring const &value) noexcept {
  InstanceSettings().MainComponentName(value);
}

bool ReactApplication::UseDeveloperSupport() noexcept {
  return InstanceSettings().UseDeveloperSupport();
}

void ReactApplication::UseDeveloperSupport(bool value) noexcept {
  InstanceSettings().UseDeveloperSupport(value);
}

hstring ReactApplication::JavaScriptMainModuleName() noexcept {
  return InstanceSettings().JavaScriptMainModuleName();
}

void ReactApplication::JavaScriptMainModuleName(hstring const &value) noexcept {
  InstanceSettings().JavaScriptMainModuleName(value);
}

hstring ReactApplication::JavaScriptBundleFile() noexcept {
  return InstanceSettings().JavaScriptBundleFile();
}

void ReactApplication::JavaScriptBundleFile(hstring const &value) noexcept {
  InstanceSettings().JavaScriptBundleFile(value);
}

void ReactApplication::OnActivated(IActivatedEventArgs const &e) {
  if (e.Kind() == ActivationKind::Protocol) {
    auto protocolActivatedEventArgs{e.as<ProtocolActivatedEventArgs>()};
    react::uwp::LinkingManagerModule::OpenUri(protocolActivatedEventArgs.Uri());
    this->OnCreate(e);
  }
}

void ReactApplication::OnLaunched(LaunchActivatedEventArgs const &e) {
  base_type::OnLaunched(e);
  // auto args = std::wstring(e.Arguments().c_str());
  this->OnCreate(e);
}

/// <summary>
/// Invoked when the application is launched normally by the end user.  Other
/// entry points will be used such as when the application is launched to open a
/// specific file.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void ReactApplication::OnCreate(IActivatedEventArgs const &e) {
  if (!m_delegate) {
    m_delegate = CreateReactApplicationDelegate();
  }

#if defined _DEBUG
  if (IsDebuggerPresent()) {
    this->DebugSettings().EnableFrameRateCounter(TRUE);

    SystemNavigationManager::GetForCurrentView().AppViewBackButtonVisibility(AppViewBackButtonVisibility::Visible);
  }
#endif

  bool isPrelaunchActivated = false;
  if (auto prelauchActivatedArgs = e.try_as<IPrelaunchActivatedEventArgs>()) {
    isPrelaunchActivated = prelauchActivatedArgs.PrelaunchActivated();
  }

  hstring args;
  if (auto lauchActivatedArgs = e.try_as<ILaunchActivatedEventArgs>()) {
    args = lauchActivatedArgs.Arguments();
  }

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

    if (!isPrelaunchActivated) {
      if (rootFrame.Content() == nullptr) {
        // When the navigation stack isn't restored navigate to the first page,
        // configuring the new page by passing required information as a
        // navigation parameter
        content = m_delegate.OnCreate(args);
        rootFrame.Content(content);
      }

      // Place the frame in the current Window
      Window::Current().Content(rootFrame);
      // Ensure the current window is active
      Window::Current().Activate();
    }
  } else {
    if (!isPrelaunchActivated) {
      if (rootFrame.Content() == nullptr) {
        // When the navigation stack isn't restored navigate to the first page,
        // configuring the new page by passing required information as a
        // navigation parameter
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
void ReactApplication::OnNavigationFailed(IInspectable const &, NavigationFailedEventArgs const &e) {
  throw hresult_error(E_FAIL, hstring(L"Failed to load Page ") + e.SourcePageType().Name);
}

ReactApplicationDelegate __stdcall ReactApplication::CreateReactApplicationDelegate() {
  return ReactApplicationDelegate(*this);
}

} // namespace winrt::Microsoft::ReactNative::implementation
