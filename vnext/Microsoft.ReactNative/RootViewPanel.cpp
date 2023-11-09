// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "RootViewPanel.h"

#include <IReactContext.h>
#include <Modules/NativeUIManager.h>
#include <Modules/PaperUIManagerModule.h>
#include <QuirkSettings.h>
#include <ReactHost/MsoUtils.h>
#include <UI.Xaml.Input.h>
#include <UI.Xaml.Media.Media3D.h>
#include <Utils/Helpers.h>
#include <dispatchQueue/dispatchQueue.h>
#include <winrt/Windows.UI.Core.h>
#include "ReactNativeHost.h"
#include "ReactViewInstance.h"
#include "TouchEventHandler.h"
#include "Views/ShadowNodeBase.h"
#include "XamlUtils.h"

// Needed for latest versions of C++/WinRT
#if __has_include("RootViewPanel.g.cpp")
#include "RootViewPanel.g.cpp"
#endif

namespace winrt::Microsoft::ReactNative::implementation {

RootViewPanel::RootViewPanel(winrt::Microsoft::ReactNative::IReactContext reactContext) noexcept {
  winrt::Microsoft::ReactNative::ReactContext reactContextImpl = reactContext;
  auto contextSelf =
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ReactContext>(reactContextImpl.Handle());

  m_touchEventHandler = std::make_shared<::Microsoft::ReactNative::TouchEventHandler>(contextSelf->GetInner());
  m_touchEventHandler->AddTouchHandlers(*this);
  m_previewKeyboardEventHandlerOnRoot =
      std::make_shared<::Microsoft::ReactNative::PreviewKeyboardEventHandlerOnRoot>(contextSelf->GetInner());
  m_previewKeyboardEventHandlerOnRoot->hook(*this);
}

RootViewPanel::~RootViewPanel() noexcept {
  if (m_touchEventHandler) {
    m_touchEventHandler->RemoveTouchHandlers();
  }

  if (m_previewKeyboardEventHandlerOnRoot) {
    m_previewKeyboardEventHandlerOnRoot->unhook();
  }

  // Clear members with a dependency on the reactInstance
  m_touchEventHandler.reset();
}

} // namespace winrt::Microsoft::ReactNative::implementation
