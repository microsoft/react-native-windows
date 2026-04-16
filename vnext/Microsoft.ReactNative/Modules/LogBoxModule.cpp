// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "LogBoxModule.h"

#include "IReactContext.h"
#include "ReactHost/ReactInstanceWin.h"
#include "ReactNativeHost.h"
#include "Utils/Helpers.h"

#include <Fabric/Composition/CompositionContextHelper.h>
#include <Fabric/Composition/CompositionUIService.h>
#include <winrt/Microsoft.UI.Windowing.h>
#include <winrt/Windows.UI.Composition.h>

namespace Microsoft::ReactNative {

LogBox::~LogBox() {
  if (m_rnWindow) {
    m_context.UIDispatcher().Post([rnWindow = m_rnWindow]() { rnWindow.Close(); });
    m_rnWindow = nullptr;
  }
}

const int LOGBOX_DEFAULT_WIDTH = 700;
const int LOGBOX_DEFAULT_HEIGHT = 1000;

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

  if (!m_rnWindow) {
    m_rnWindow = winrt::Microsoft::ReactNative::ReactNativeWindow::CreateFromCompositor(
        winrt::Microsoft::ReactNative::Composition::CompositionUIService::GetCompositor(
            host.InstanceSettings().Properties()));
    winrt::Microsoft::ReactNative::ReactViewOptions viewOptions;
    viewOptions.ComponentName(L"LogBox");
    m_rnWindow.AppWindow().Title(L"LogBox");
    m_rnWindow.AppWindow().Closing([](winrt::Microsoft::UI::Windowing::AppWindow const & /*appWindow*/,
                                      winrt::Microsoft::UI::Windowing::AppWindowClosingEventArgs const &args) {
      // Prevent default close behavior - as JS will never try to show the logbox again if it isn't hidden through JS
      args.Cancel(true);
    });
    m_rnWindow.ReactNativeIsland().ReactViewHost(
        winrt::Microsoft::ReactNative::ReactCoreInjection::MakeViewHost(host, viewOptions));
    m_rnWindow.AppWindow().ResizeClient({LOGBOX_DEFAULT_WIDTH, LOGBOX_DEFAULT_HEIGHT});
  }

  m_rnWindow.AppWindow().Show(true);
  m_rnWindow.AppWindow().MoveInZOrderAtTop();
}

void LogBox::HideOnUIThread() noexcept {
  if (m_rnWindow) {
    m_rnWindow.AppWindow().Hide();
  }
}

void LogBox::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

} // namespace Microsoft::ReactNative
