// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <winrt/Windows.ApplicationModel.Activation.h>

// ReactApplicationDelegate.h must be included before ReactApplicationDelegate.g.cpp
#include "ReactApplicationDelegate.h"

#include "ReactApplicationDelegate.g.cpp"
#include "ReactNativeHost.h"
#include "ReactSupport.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;

namespace winrt::Microsoft::ReactNative::implementation {

static void ApplyArguments(ReactNative::ReactNativeHost const &host, std::wstring const &arguments) noexcept {
  Microsoft::ReactNative::implementation::ReactNativeHost *hostImpl{
      get_self<Microsoft::ReactNative::implementation::ReactNativeHost>(host)};
  if (!arguments.empty() /*&& host.HasInstance()*/) {
    constexpr wchar_t delimiter = L' ';
    std::wistringstream argumentStream(arguments);
    std::wstring token;
    while (std::getline(argumentStream, token, delimiter)) {
      if (token == L"-?") {
        std::cout << "Options:" << std::endl
                  << "  --direct-debugging <port>    Enable direct debugging on specified port." << std::endl;
      } else if (token == L"--direct-debugging") {
        if (std::getline(argumentStream, token, delimiter)) {
          const uint16_t port = static_cast<uint16_t>(std::wcstoul(token.c_str(), nullptr, 10));
          hostImpl->InstanceSettings().UseWebDebugger(false);
          hostImpl->InstanceSettings().UseDirectDebugger(true);
          hostImpl->InstanceSettings().DebuggerBreakOnNextLine(true);
          hostImpl->InstanceSettings().DebuggerPort(port);
        }
      }
    }
    // TODO: check for 'remoteDebugging'.  Return if not found.  Otherwise,
    // validate a value is provided and then parse it to set the
    // ReactInstanceManager.DevSupportManager.IsRemoteDebuggingEnabled flag
  }
}

ReactApplicationDelegate::ReactApplicationDelegate(Application const &application) noexcept
    : m_application(application) {
  VerifyElseCrash(application);

  m_reactApplication = application.as<IReactApplication>();
  VerifyElseCrashSz(m_reactApplication, "Expected argument to implement 'IReactApplication' interface");

  m_application.Resuming({this, &ReactApplicationDelegate::OnResuming});
  m_application.Suspending({this, &ReactApplicationDelegate::OnSuspending});
  m_application.LeavingBackground({this, &ReactApplicationDelegate::OnLeavingBackground});
  m_application.EnteredBackground({this, &ReactApplicationDelegate::OnEnteredBackground});
}

void ReactApplicationDelegate::OnActivated(IActivatedEventArgs const &args) noexcept {
  switch (args.Kind()) {
    case ActivationKind::Protocol:
      auto protocolArgs = args.as<IProtocolActivatedEventArgs>();
      // auto uri = protocolArgs.Uri;

      // TODO: Need to support deep linking by integrating with the Linking module
      if (args.PreviousExecutionState() != ApplicationExecutionState::Running) {
        // TODO... Figure out the right activation path for PreviousExecutionState
        VerifyElseCrashSz(
            false, "ReactApplicationDelegate.OnActivated doesn't handle PreviousExecutionState other than Running");
      } else {
        // TODO... Figure out the right activation path
        OutputDebugStringW(
            L"ReactApplicationDelegate.OnActivated doesn't handle PreviousExecutionState when its Running");
      }
      break;
  }
}

// Create the root view for the ReactNative app
UIElement ReactApplicationDelegate::OnCreate(hstring const &arguments) noexcept {
  auto host = m_reactApplication.Host();

  ApplyArguments(host, arguments.c_str());

  if (m_reactRootView != nullptr) {
    return *m_reactRootView;
  }

  // Nudge the ReactNativeHost to create the instance and wrapping context
  host.ReloadInstance();

  m_reactRootView = winrt::make_self<ReactRootView>();
  m_reactRootView->ComponentName(host.InstanceSettings().MainComponentName());
  m_reactRootView->ReactNativeHost(host);

  auto resources = Application::Current().Resources();
  auto brush = resources.Lookup(box_value(L"ApplicationPageBackgroundThemeBrush")).as<Media::SolidColorBrush>();
  m_reactRootView->Background(brush);

  return *m_reactRootView;
}

void ReactApplicationDelegate::OnResuming(IInspectable const & /*sender*/, IInspectable const & /*args*/) noexcept {
  OutputDebugStringW(L"ReactApplicationDelegate::OnResuming");
}

void ReactApplicationDelegate::OnSuspending(IInspectable const & /*sender*/, IInspectable const & /*args*/) noexcept {
  OutputDebugStringW(L"ReactApplicationDelegate::OnSuspending");
}

void ReactApplicationDelegate::OnLeavingBackground(
    IInspectable const & /*sender*/,
    LeavingBackgroundEventArgs const & /*args*/) noexcept {
  OutputDebugStringW(L"ReactApplicationDelegate::OnLeavingBackground");
}

void ReactApplicationDelegate::OnEnteredBackground(
    IInspectable const & /*sender*/,
    EnteredBackgroundEventArgs const & /*args*/) noexcept {
  OutputDebugStringW(L"ReactApplicationDelegate::OnEnteredBackground");
}

} // namespace winrt::Microsoft::ReactNative::implementation
