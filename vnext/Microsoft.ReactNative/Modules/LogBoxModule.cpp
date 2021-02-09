// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "LogBoxModule.h"

#include "IReactContext.h"
#include "ReactHost/ReactInstanceWin.h"
#include "ReactNativeHost.h"
#include "Utils/Helpers.h"

#include <UI.Xaml.Controls.Primitives.h>

namespace Microsoft::ReactNative {

void LogBox::Show() noexcept {
  if (!Mso::React::ReactOptions::UseDeveloperSupport(m_context.Properties().Handle())) {
    return;
  }

  m_context.UIDispatcher().Post([weakThis = weak_from_this()] {
    if (auto strongThis = weakThis.lock()) {
      strongThis->ShowOnUIThread();
    }
  });
}

void LogBox::Hide() noexcept {
  m_context.UIDispatcher().Post([weakThis = weak_from_this()] {
    if (auto strongThis = weakThis.lock()) {
      strongThis->HideOnUIThread();
    }
  });
}

void LogBox::ShowOnUIThread() noexcept {
  auto host = React::implementation::ReactNativeHost::GetReactNativeHost(m_context.Properties());
  if (!host)
    return;

  m_logBoxContent = React::ReactRootView();
  m_logBoxContent.ComponentName(L"LogBox");
  m_logBoxContent.ReactNativeHost(host);

  m_popup = xaml::Controls::Primitives::Popup{};
  xaml::FrameworkElement root{nullptr};

  if (react::uwp::Is19H1OrHigher()) {
    // XamlRoot added in 19H1 - is required to be set for XamlIsland scenarios
    if (auto xamlRoot = React::XamlUIService::GetXamlRoot(m_context.Properties().Handle())) {
      m_popup.XamlRoot(xamlRoot);
      root = xamlRoot.Content().as<xaml::FrameworkElement>();
    }
  }

  if (!root) {
    auto window = xaml::Window::Current();
    root = window.Content().as<xaml::FrameworkElement>();
  }

  m_logBoxContent.MaxHeight(root.ActualHeight());
  m_logBoxContent.Height(root.ActualHeight());
  m_logBoxContent.MaxWidth(root.ActualWidth());
  m_logBoxContent.Width(root.ActualWidth());
  m_logBoxContent.UpdateLayout();

  m_sizeChangedRevoker = root.SizeChanged(
      winrt::auto_revoke, [wkThis = weak_from_this()](auto const & /*sender*/, xaml::SizeChangedEventArgs const &args) {
        if (auto strongThis = wkThis.lock()) {
          strongThis->m_logBoxContent.MaxHeight(args.NewSize().Height);
          strongThis->m_logBoxContent.Height(args.NewSize().Height);
          strongThis->m_logBoxContent.MaxWidth(args.NewSize().Width);
          strongThis->m_logBoxContent.Width(args.NewSize().Width);
        }
      });

  m_tokenClosed = m_popup.Closed(
      [wkThis = weak_from_this()](auto const & /*sender*/, winrt::IInspectable const & /*args*/) noexcept {
        if (auto strongThis = wkThis.lock()) {
          strongThis->HideOnUIThread();
        }
      });

  m_popup.Child(m_logBoxContent);
  m_popup.IsOpen(true);
}

void LogBox::HideOnUIThread() noexcept {
  if (m_popup) {
    m_popup.Closed(m_tokenClosed);
    m_sizeChangedRevoker.revoke();
    m_popup.IsOpen(false);
    m_popup = nullptr;
    m_logBoxContent = nullptr;
  }
}

void LogBox::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

} // namespace Microsoft::ReactNative
