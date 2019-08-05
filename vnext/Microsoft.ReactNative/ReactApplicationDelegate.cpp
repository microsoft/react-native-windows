// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactApplicationDelegate.h"
#if __has_include("ReactApplicationDelegate.g.cpp")
#include "ReactApplicationDelegate.g.cpp"
#endif
#include "ReactNativeHost.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;

namespace {

static void ApplyArguments(
    winrt::Microsoft::ReactNative::ReactNativeHost host,
    std::wstring arguments) {
   //Microsoft::ReactNative::implementation::ReactNativeHost* hostImpl {
   //get_self<Microsoft::ReactNative::implementation::ReactNativeHost>(host)};
  if (!arguments.empty() && host.HasInstance()) {
    // TODO: check for 'remoteDebugging'.  Return if not found.  Otherwise,
    // validate a value is provided and then parse it to set the
    // ReactInstanceManager.DevSupportManager.IsRemoteDebuggingEnabled flag
  }
}
} // namespace

namespace winrt::Microsoft::ReactNative::implementation {
ReactApplicationDelegate::ReactApplicationDelegate(
    Application const &application)
    : m_application(application) {
  if (application == nullptr) {
    throw winrt::hresult_null_argument(); // ArgumentNullException
  }

  m_reactApplication = application.as<IReactApplication>();
  if (m_reactApplication == nullptr) {
    throw winrt::hresult_invalid_argument(
        L"Expected argument to implement 'IReactApplication' interface");
  }

  m_application.Resuming({this, &ReactApplicationDelegate::OnResuming});
  m_application.Suspending({this, &ReactApplicationDelegate::OnSuspending});
  m_application.LeavingBackground(
      {this, &ReactApplicationDelegate::OnLeavingBackground});
  m_application.EnteredBackground(
      {this, &ReactApplicationDelegate::OnEnteredBackground});
}

void ReactApplicationDelegate::OnActivated(
    winrt::Windows::ApplicationModel::Activation::IActivatedEventArgs const
        &args) {
  switch (args.Kind()) {
    case ActivationKind::Protocol:
      auto protocolArgs = args.as<IProtocolActivatedEventArgs>();
      // auto uri = protocolArgs.Uri;

      // TODO: Need to support deep linking by integrating with the Linking module
      throw winrt::hresult_not_implemented(
          L"Integration with Linking not yet implemented.");
      if (args.PreviousExecutionState() != ApplicationExecutionState::Running) {
        // TODO... Figure out the right activation path for PreviousExecutionState
        throw winrt::hresult_not_implemented(
            L"ReactApplicationDelegate.OnActivated doesn't handle PreviousExecutionState other than Running");
      } else {
        // TODO... Figure out the right activation path
        throw winrt::hresult_not_implemented(
            L"ReactApplicationDelegate.OnActivated doesn't handle PreviousExecutionState when its Running");
      }
      break;
  }
}

// Create the root view for the ReactNative app
UIElement ReactApplicationDelegate::OnCreate(hstring const &arguments) {
  auto host = m_reactApplication.Host();
  host.OnResume([=]() { m_application.Exit(); });

  ApplyArguments(host, arguments.c_str());
  return host.GetOrCreateRootView(nullptr);
}

void ReactApplicationDelegate::OnResuming(
    IInspectable sender,
    IInspectable args) {
  m_reactApplication.Host().OnResume([=]() { m_application.Exit(); });

  OutputDebugStringW(L"ReactApplicationDelegate::OnResuming");
}

void ReactApplicationDelegate::OnSuspending(
    IInspectable sender,
    IInspectable args) {
  OutputDebugStringW(L"ReactApplicationDelegate::OnSuspending");
  m_reactApplication.Host().OnSuspend();
}

void ReactApplicationDelegate::OnLeavingBackground(
    IInspectable sender,
    winrt::Windows::ApplicationModel::LeavingBackgroundEventArgs args) {
  OutputDebugStringW(L"ReactApplicationDelegate::OnLeavingBackground");
  m_reactApplication.Host().OnLeavingBackground();
}

void ReactApplicationDelegate::OnEnteredBackground(
    IInspectable sender,
    winrt::Windows::ApplicationModel::EnteredBackgroundEventArgs args) {
  OutputDebugStringW(L"ReactApplicationDelegate::OnEnteredBackground");
  m_reactApplication.Host().OnEnteredBackground();
}
} // namespace winrt::Microsoft::ReactNative::implementation
