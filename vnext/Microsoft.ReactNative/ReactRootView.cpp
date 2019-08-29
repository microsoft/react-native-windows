// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "pch.h"
#include "ReactRootView.h"
#if __has_include("ReactRootView.g.cpp")
#include "ReactRootView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;

namespace winrt::Microsoft::ReactNative::implementation {
void ReactRootView::OnCreate(
    Microsoft::ReactNative::ReactNativeHost const &host) {
  auto resources = Application::Current().Resources();
  auto brush =
      resources.Lookup(box_value(L"ApplicationPageBackgroundThemeBrush"))
          .as<Media::SolidColorBrush>();
  this->Background(brush);

  if (Dispatcher().HasThreadAccess()) {
    SystemNavigationManager::GetForCurrentView().BackRequested(
        [host = std::move(host)](
            IInspectable sender, BackRequestedEventArgs e) {
          ReactRootView::OnBackRequested(host, sender, e);
        });

    Window::Current().CoreWindow().Dispatcher().AcceleratorKeyActivated(
        [host = std::move(host)](
            CoreDispatcher sender, AcceleratorKeyEventArgs e) {
          ReactRootView::OnAcceleratorKeyActivated(host, sender, e);
        });
  }
}

fire_and_forget ReactRootView::StartReactApplicationAsync(
    Microsoft::ReactNative::ReactInstanceManager const &instanceManager,
    hstring componentName,
    folly::dynamic initialProps) {
  if (!Dispatcher().HasThreadAccess()) {
    throw hresult_invalid_operation(L"Thread does not have dispatcher access.");
  }

  if (m_reactInstanceManager != nullptr) {
    throw hresult_invalid_operation(
        L"This root view has already been attached to an instance manager.");
  }

  m_reactInstanceManager = instanceManager;
  m_moduleName = componentName;
  m_initialProps = initialProps;

  // Nudge the ReactInstanceManager to create the instance and wrapping context
  ReactInstanceManager *instanceManagerImpl{
      get_self<ReactInstanceManager>(instanceManager)};

  auto context = co_await instanceManagerImpl->GetOrCreateReactContextAsync();

  auto instanceCreator = instanceManagerImpl->InstanceCreator();

  m_xamlView = react::uwp::CreateReactRootView(
      *this, componentName.c_str(), instanceCreator);

  if (m_xamlView == nullptr)
    co_return;

  // Set initial properties since just created
  m_xamlView->SetInitialProps(std::move(initialProps));

  m_xamlView->AttachRoot();
}

void ReactRootView::OnBackRequested(
    ReactNative::ReactNativeHost const &host,
    IInspectable const & /*sender*/,
    BackRequestedEventArgs const &e) {
  auto hostImpl = host.as<ReactNativeHost>();
  if (hostImpl->HasInstance()) {
    hostImpl->ReactInstanceManager().OnBackPressed();
    e.Handled(TRUE);
  }
}

void ReactRootView::OnAcceleratorKeyActivated(
    ReactNative::ReactNativeHost const &host,
    CoreDispatcher const & /*sender*/,
    AcceleratorKeyEventArgs const & /*e*/) {
  // TODO: Add DevSupportManager which may also be the right place for
  // some of the properties currently on the ReactInstanceSettings.
  // It should also be wired up to a Developer menu that can be
  // accessed in debug via the Shift+F10 key.  Consider what was done in
  // https://github.com/microsoft/react-native-windows/pull/2862/files
  // and the C# DevSupportManager.
  if (host.HasInstance()) {
    // auto reactInstanceManager = host.ReactInstanceManager();
    // if (reactInstanceManager.DevSupportManager().IsEnabled()) {

    //  auto virtualKey = e.VirtualKey();
    //  auto eventType = e.EventType();

    //  if (virtualKey == Windows::System::VirtualKey::Shift) {
    //    s_isShiftKeyDown = IsKeyDown(eventType);
    //  }
    //  else if (virtualKey == Windows::System::VirtualKey::Control) {
    //    s_isControlKeyDown = IsKeyDown(eventType);
    //  }
    //  else if (IsKeyDown(eventType)
    //    && s_isShiftKeyDown
    //    && virtualKey == Windows::System::VirtualKey::F10) {
    //    reactInstanceManager.DevSupportManager().ShowDevOptionsDialog();
    //  }
    //  else if (
    //    eventType == CoreAcceleratorKeyEventType::KeyUp
    //    && s_isControlKeyDown
    //    && e.VirtualKey == Windows::System::VirtualKey::R) {
    //    reactInstanceManager.DevSupportManager().HandleReloadJavaScript();
    //  }
    //}
  }
}

bool ReactRootView::IsKeyDown(CoreAcceleratorKeyEventType t) {
  return t == CoreAcceleratorKeyEventType::KeyDown ||
      t == CoreAcceleratorKeyEventType::SystemKeyDown;
}
} // namespace winrt::Microsoft::ReactNative::implementation
