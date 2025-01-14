// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "DeviceInfoModule.h"
#include <DesktopWindowBridge.h>
#include <IReactDispatcher.h>
#include <IReactPropertyBag.h>
#ifndef USE_WINUI3
#include <UI.Xaml.Hosting.DesktopWindowXamlSource.h>
#endif
#include <Utils/Helpers.h>
#include <XamlUIService.h>
#include <XamlUtils.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.ViewManagement.h>

#ifdef USE_FABRIC
#include <ReactCoreInjection.h>
#endif

using namespace winrt::Microsoft::ReactNative;

namespace Microsoft::ReactNative {

static const React::ReactPropertyId<React::ReactNonAbiValue<std::shared_ptr<DeviceInfoHolder>>>
    &DeviceInfoHolderPropertyId() noexcept {
  static const React::ReactPropertyId<React::ReactNonAbiValue<std::shared_ptr<DeviceInfoHolder>>> prop{
      L"ReactNative.DeviceInfo", L"DeviceInfoHolder"};
  return prop;
}

DeviceInfoHolder::DeviceInfoHolder(const Mso::React::IReactContext &context) : m_context(&context) {
  updateDeviceInfo();
}

void DeviceInfoHolder::InitDeviceInfoHolder(const Mso::React::IReactContext &context) noexcept {
  if (xaml::TryGetCurrentUwpXamlApplication() || IsFabricEnabled(context.Properties())) {
    auto deviceInfoHolder = std::make_shared<DeviceInfoHolder>(context);

    deviceInfoHolder->updateDeviceInfo();
    winrt::Microsoft::ReactNative::ReactPropertyBag pb{context.Properties()};
    pb.Set(DeviceInfoHolderPropertyId(), std::move(deviceInfoHolder));

    uint64_t hwnd = 0;

#ifdef USE_FABRIC
    if (IsFabricEnabled(context.Properties())) {
      hwnd = winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::GetTopLevelWindowId(pb.Handle());
    }
#endif

    if (xaml::TryGetCurrentUwpXamlApplication()) {
      if (auto window = xaml::Window::Current()) {
        auto const &coreWindow = window.CoreWindow();

        deviceInfoHolder->m_sizeChangedRevoker = coreWindow.SizeChanged(
            winrt::auto_revoke, [weakHolder = std::weak_ptr(deviceInfoHolder)](auto &&, auto &&) {
              if (auto strongHolder = weakHolder.lock()) {
                strongHolder->updateDeviceInfo();
              }
            });
      } else {
        assert(IsXamlIsland());
        // This is either a WinUI 3 island or a system XAML island
        // system XAML islands have a CoreWindow so we want to use the GetForCurrentView APIs
        // For WinUI 3 islands we require the app to forward window messages as ReactNotifications
      }

      if (!IsWinUI3Island()) {
        // UWP or system XAML island
        auto const &displayInfo = winrt::Windows::Graphics::Display::DisplayInformation::GetForCurrentView();

        deviceInfoHolder->m_dpiChangedRevoker = displayInfo.DpiChanged(
            winrt::auto_revoke, [weakHolder = std::weak_ptr(deviceInfoHolder)](const auto &, const auto &) {
              if (auto strongHolder = weakHolder.lock()) {
                strongHolder->updateDeviceInfo();
              }
            });
      } else {
        hwnd = XamlUIService::GetIslandWindowHandle(deviceInfoHolder->m_context->Properties());
      }
    }

    if (hwnd) {
      deviceInfoHolder->m_wmSubscription = SubscribeToWindowMessage(
          ReactNotificationService(context.Notifications()),
          WM_WINDOWPOSCHANGED,
          [weakHolder = std::weak_ptr(deviceInfoHolder)](HWND hwnd, const DesktopWindowMessage &dwm) {
            if (auto strongHolder = weakHolder.lock()) {
              const auto pos = reinterpret_cast<WINDOWPOS *>(dwm.LParam);
              const auto newWidth = static_cast<float>(pos->cx);
              const auto newHeight = static_cast<float>(pos->cy);
              const auto changed =
                  (strongHolder->m_windowWidth != newWidth) || (strongHolder->m_windowHeight != newHeight);
              strongHolder->m_windowWidth = newWidth;
              strongHolder->m_windowHeight = newHeight;
              if (changed) {
                strongHolder->notifyChanged();
              }
            }
          });
    }
  }
}

void DeviceInfoHolder::notifyChanged() noexcept {
  if (m_notifyCallback) {
    auto writer = MakeJSValueTreeWriter();
    WriteValue(writer, getDimensions());
    m_notifyCallback(TakeJSValue(writer).MoveObject());
  }
}

ReactNativeSpecs::DeviceInfoSpec_DimensionsPayload DeviceInfoHolder::GetDimensions(
    const React::ReactPropertyBag &propertyBag) noexcept {
  auto holder = propertyBag.Get(DeviceInfoHolderPropertyId());
  return (*holder)->getDimensions();
}

ReactNativeSpecs::DeviceInfoSpec_DisplayMetricsAndroid DeviceInfoHolder::getWindow() noexcept {
  ReactNativeSpecs::DeviceInfoSpec_DisplayMetricsAndroid metrics;
  metrics.width = m_windowWidth * m_scale;
  metrics.height = m_windowHeight * m_scale;
  metrics.scale = m_scale;
  metrics.fontScale = m_textScaleFactor;
  metrics.densityDpi = m_dpi;
  return metrics;
}

ReactNativeSpecs::DeviceInfoSpec_DisplayMetricsAndroid DeviceInfoHolder::getScreen() noexcept {
  ReactNativeSpecs::DeviceInfoSpec_DisplayMetricsAndroid metrics;
  metrics.width = m_screenWidth;
  metrics.height = m_screenHeight;
  metrics.scale = m_scale;
  metrics.fontScale = m_textScaleFactor;
  metrics.densityDpi = m_dpi;
  return metrics;
}

ReactNativeSpecs::DeviceInfoSpec_DimensionsPayload DeviceInfoHolder::getDimensions() noexcept {
  ReactNativeSpecs::DeviceInfoSpec_DimensionsPayload payload;
  payload.windowPhysicalPixels = getWindow();
  payload.screenPhysicalPixels = getScreen();
  return payload;
}

void DeviceInfoHolder::SetCallback(
    const React::ReactPropertyBag &propertyBag,
    Mso::Functor<void(React::JSValueObject &&)> &&callback) noexcept {
  auto holder = propertyBag.Get(DeviceInfoHolderPropertyId());

  if (holder) {
    (*holder)->m_notifyCallback = std::move(callback);
  }
}

void DeviceInfoHolder::updateDeviceInfo() noexcept {
  if (xaml::TryGetCurrentUwpXamlApplication() && xaml::Window::Current()) {
    auto const window = xaml::Window::Current().CoreWindow();

    m_windowWidth = window.Bounds().Width;
    m_windowHeight = window.Bounds().Height;
  } else {
    auto hwnd = XamlUIService::GetIslandWindowHandle(m_context->Properties());

#ifdef USE_FABRIC
    if (IsFabricEnabled(m_context->Properties())) {
      winrt::Microsoft::ReactNative::ReactPropertyBag pb{m_context->Properties()};
      hwnd = winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::GetTopLevelWindowId(pb.Handle());
    }
#endif

    if (hwnd) {
      RECT rect{};
      if (CALL_INDIRECT(L"user32.dll", GetWindowRect, reinterpret_cast<HWND>(hwnd), &rect)) {
        m_windowWidth = (float)(rect.right - rect.left);
        m_windowHeight = (float)(rect.bottom - rect.top);
      }
    }
  }

  winrt::Windows::UI::ViewManagement::UISettings uiSettings;
  m_textScaleFactor = uiSettings.TextScaleFactor();
  if (!IsWinUI3Island()) {
    auto const displayInfo = winrt::Windows::Graphics::Display::DisplayInformation::GetForCurrentView();
    m_scale = static_cast<float>(displayInfo.ResolutionScale()) / 100;
    m_dpi = displayInfo.LogicalDpi();
    m_screenWidth = displayInfo.ScreenWidthInRawPixels();
    m_screenHeight = displayInfo.ScreenHeightInRawPixels();
    notifyChanged();
  } else {
    RECT desktopRect{};

    auto hwnd = XamlUIService::GetIslandWindowHandle(m_context->Properties());

#ifdef USE_FABRIC
    if (IsFabricEnabled(m_context->Properties())) {
      winrt::Microsoft::ReactNative::ReactPropertyBag pb{m_context->Properties()};
      hwnd = winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::GetTopLevelWindowId(pb.Handle());
    }
#endif

    if (hwnd && CALL_INDIRECT(L"user32.dll", GetWindowRect, reinterpret_cast<HWND>(hwnd), &desktopRect)) {
      m_screenWidth = static_cast<uint32_t>(desktopRect.right - desktopRect.left);
      m_screenHeight = static_cast<uint32_t>(desktopRect.bottom - desktopRect.top);
      m_dpi = static_cast<float>(CALL_INDIRECT(L"user32.dll", GetDpiForWindow, reinterpret_cast<HWND>(hwnd)));
    }
  }
}

ReactNativeSpecs::DeviceInfoSpec_DeviceInfoConstants DeviceInfo::GetConstants() noexcept {
  ReactNativeSpecs::DeviceInfoSpec_DeviceInfoConstants constants;
  constants.Dimensions = DeviceInfoHolder::GetDimensions(m_context.Properties());
  return constants;
}

void DeviceInfo::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;

  DeviceInfoHolder::SetCallback(
      m_context.Properties(), [weakThis = weak_from_this()](React::JSValueObject &&dimensions) {
        if (auto strongThis = weakThis.lock()) {
          strongThis->m_context.EmitJSEvent(L"RCTDeviceEventEmitter", L"didUpdateDimensions", dimensions);
        }
      });
}

} // namespace Microsoft::ReactNative
