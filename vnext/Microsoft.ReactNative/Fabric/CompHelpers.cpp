
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "CompHelpers.h"
#include <Utils/ValueUtils.h>
#include <Windows.Graphics.DirectX.Direct3D11.interop.h>
#include <d2d1_1.h>
#include <d3d11.h>
#include <d3d11_4.h>
#include <dwrite.h>
#include <unicode.h>
#include <windows.ui.composition.interop.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>

namespace Microsoft::ReactNative {

bool CheckForDeviceRemoved(HRESULT hr) {
  if (SUCCEEDED(hr)) {
    // Everything is fine -- go ahead and draw
    return true;
  } else if (hr == DXGI_ERROR_DEVICE_REMOVED) {
    // We can't draw at this time, but this failure is recoverable. Just skip drawing for
    // now. We will be asked to draw again once the Direct3D device is recreated
    return false;
  } else {
    // Any other error is unexpected and, therefore, fatal
    std::terminate();
  }
}

struct DeviceLostEventArgs {
  DeviceLostEventArgs(winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice const &device)
      : m_device(device) {}
  winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice Device() {
    return m_device;
  }
  static DeviceLostEventArgs Create(winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice const &device) {
    return DeviceLostEventArgs{device};
  }

 private:
  winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice m_device;
};

struct DeviceLostHelper {
  DeviceLostHelper() = default;

  ~DeviceLostHelper() {
    StopWatchingCurrentDevice();
    m_onDeviceLostHandler = nullptr;
  }

  winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice CurrentlyWatchedDevice() {
    return m_device;
  }

  void WatchDevice(winrt::com_ptr<::IDXGIDevice> const &dxgiDevice) {
    // If we're currently listening to a device, then stop.
    StopWatchingCurrentDevice();

    // Set the current device to the new device.
    m_device = nullptr;
    winrt::check_hresult(::CreateDirect3D11DeviceFromDXGIDevice(
        dxgiDevice.get(), reinterpret_cast<::IInspectable **>(winrt::put_abi(m_device))));

    // Get the DXGI Device.
    m_dxgiDevice = dxgiDevice;

    // QI For the ID3D11Device4 interface.
    winrt::com_ptr<::ID3D11Device4> d3dDevice{m_dxgiDevice.as<::ID3D11Device4>()};

    // Create a wait struct.
    m_onDeviceLostHandler = nullptr;
    m_onDeviceLostHandler = ::CreateThreadpoolWait(DeviceLostHelper::OnDeviceLost, (PVOID)this, nullptr);

    // Create a handle and a cookie.
    m_eventHandle.attach(::CreateEvent(nullptr, false, false, nullptr));
    winrt::check_bool(bool{m_eventHandle});
    m_cookie = 0;

    // Register for device lost.
    ::SetThreadpoolWait(m_onDeviceLostHandler, m_eventHandle.get(), nullptr);
    winrt::check_hresult(d3dDevice->RegisterDeviceRemovedEvent(m_eventHandle.get(), &m_cookie));
  }

  void StopWatchingCurrentDevice() {
    if (m_dxgiDevice) {
      // QI For the ID3D11Device4 interface.
      auto d3dDevice{m_dxgiDevice.as<::ID3D11Device4>()};

      // Unregister from the device lost event.
      ::CloseThreadpoolWait(m_onDeviceLostHandler);
      d3dDevice->UnregisterDeviceRemoved(m_cookie);

      // Clear member variables.
      m_onDeviceLostHandler = nullptr;
      m_eventHandle.close();
      m_cookie = 0;
      m_device = nullptr;
    }
  }

  void DeviceLost(winrt::delegate<DeviceLostHelper const *, DeviceLostEventArgs const &> const &handler) {
    m_deviceLost = handler;
  }

  winrt::delegate<DeviceLostHelper const *, DeviceLostEventArgs const &> m_deviceLost;

 private:
  void RaiseDeviceLostEvent(winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice const &oldDevice) {
    m_deviceLost(this, DeviceLostEventArgs::Create(oldDevice));
  }

  static void CALLBACK OnDeviceLost(
      PTP_CALLBACK_INSTANCE /* instance */,
      PVOID context,
      PTP_WAIT /* wait */,
      TP_WAIT_RESULT /* waitResult */) {
    auto deviceLostHelper = reinterpret_cast<DeviceLostHelper *>(context);
    auto oldDevice = deviceLostHelper->m_device;
    deviceLostHelper->StopWatchingCurrentDevice();
    deviceLostHelper->RaiseDeviceLostEvent(oldDevice);
  }

 private:
  winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice m_device;
  winrt::com_ptr<::IDXGIDevice> m_dxgiDevice;
  PTP_WAIT m_onDeviceLostHandler{nullptr};
  winrt::handle m_eventHandle;
  DWORD m_cookie{0};
};

CompContext::CompContext(winrt::Windows::UI::Composition::Compositor const &compositor) : m_compositor(compositor) {}

winrt::Windows::UI::Composition::Compositor CompContext::Compositor() const noexcept {
  return m_compositor;
}

winrt::com_ptr<ID2D1Factory1> CompContext::D2DFactory() noexcept {
  if (!m_d2dFactory) {
    // Initialize Direct2D resources.
    D2D1_FACTORY_OPTIONS d2d1FactoryOptions{D2D1_DEBUG_LEVEL_NONE};

    D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &d2d1FactoryOptions, m_d2dFactory.put_void());
  }
  return m_d2dFactory;
}
winrt::com_ptr<ID3D11Device> CompContext::D3DDevice() noexcept {
  // This flag adds support for surfaces with a different color channel ordering than the API default.
  // You need it for compatibility with Direct2D.
  UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

  // This array defines the set of DirectX hardware feature levels this app  supports.
  // The ordering is important and you should  preserve it.
  // Don't forget to declare your app's minimum required feature level in its
  // description.  All apps are assumed to support 9.1 unless otherwise stated.
  D3D_FEATURE_LEVEL featureLevels[] = {
      D3D_FEATURE_LEVEL_11_1,
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_1,
      D3D_FEATURE_LEVEL_10_0,
      D3D_FEATURE_LEVEL_9_3,
      D3D_FEATURE_LEVEL_9_2,
      D3D_FEATURE_LEVEL_9_1};
  if (!m_d3dDevice) {
    D3D11CreateDevice(
        nullptr, // specify null to use the default adapter
        D3D_DRIVER_TYPE_HARDWARE,
        0,
        creationFlags, // optionally set debug and Direct2D compatibility flags
        featureLevels, // list of feature levels this app can support
        ARRAYSIZE(featureLevels), // number of possible feature levels
        D3D11_SDK_VERSION,
        m_d3dDevice.put(), // returns the Direct3D device created
        nullptr /*&m_featureLevel*/, // returns feature level of device created
        nullptr /*&context*/ // returns the device immediate context
    );
  }
  return m_d3dDevice;
}

winrt::com_ptr<ID2D1Device> CompContext::D2DDevice() noexcept {
  if (!m_d2dDevice) {
    winrt::com_ptr<IDXGIDevice> dxgiDevice;
    // Obtain the underlying DXGI device of the Direct3D11 device.
    D3DDevice().as(dxgiDevice);

    // Obtain the Direct2D device for 2-D rendering.
    winrt::check_hresult(D2DFactory()->CreateDevice(dxgiDevice.get(), m_d2dDevice.put()));
  }
  return m_d2dDevice;
}

winrt::Windows::UI::Composition::CompositionGraphicsDevice CompContext::CompositionGraphicsDevice() noexcept {
  if (!m_compositionGraphicsDevice) {
    // To create a composition graphics device, we need to QI for another interface

    winrt::com_ptr<ABI::Windows::UI::Composition::ICompositorInterop> compositorInterop{
        m_compositor.as<ABI::Windows::UI::Composition::ICompositorInterop>()};

    // Create a graphics device backed by our D3D device
    winrt::com_ptr<ABI::Windows::UI::Composition::ICompositionGraphicsDevice> compositionGraphicsDeviceIface;
    winrt::check_hresult(
        compositorInterop->CreateGraphicsDevice(D2DDevice().get(), compositionGraphicsDeviceIface.put()));

    compositionGraphicsDeviceIface.as(m_compositionGraphicsDevice);
  }
  return m_compositionGraphicsDevice;
}

winrt::com_ptr<::IDWriteFactory> DWriteFactory() noexcept {
  static winrt::com_ptr<::IDWriteFactory> s_dwriteFactory;
  if (!s_dwriteFactory) {
    winrt::check_hresult(::DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED, __uuidof(s_dwriteFactory), reinterpret_cast<::IUnknown **>(s_dwriteFactory.put())));
  }
  return s_dwriteFactory;
}

} // namespace Microsoft::ReactNative
