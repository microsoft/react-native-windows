// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <winrt/Microsoft.ReactNative.Composition.Experimental.h>
#include <winrt/Microsoft.ReactNative.Composition.h>

#include <CompositionSwitcher.Experimental.interop.h>

namespace Microsoft::ReactNative::Composition {

class AutoDrawDrawingSurface {
 public:
  AutoDrawDrawingSurface(
      winrt::Microsoft::ReactNative::Composition::Experimental::IDrawingSurfaceBrush &drawingSurface,
      float scaleFactor,
      POINT *offset) noexcept {
    drawingSurface.as(m_drawingSurfaceInterop);
    auto dpi = scaleFactor * 96.0f;
    auto hr = m_drawingSurfaceInterop->BeginDraw(m_d2dDeviceContext.put(), dpi, dpi, offset);
    if (FAILED(hr)) {
      m_d2dDeviceContext = nullptr;
    }
  }

  ~AutoDrawDrawingSurface() noexcept {
    if (m_d2dDeviceContext) {
      m_d2dDeviceContext = nullptr;
      m_drawingSurfaceInterop->EndDraw();
    }
  }

  operator ID2D1DeviceContext *() noexcept {
    return m_d2dDeviceContext.get();
  }

  ID2D1DeviceContext *GetRenderTarget() noexcept {
    return m_d2dDeviceContext.get();
  }

  // Returns true if the device context is valid and safe to use for drawing operations
  bool IsValid() const noexcept {
    return m_d2dDeviceContext != nullptr;
  }

 private:
  winrt::com_ptr<Experimental::ICompositionDrawingSurfaceInterop> m_drawingSurfaceInterop;
  winrt::com_ptr<ID2D1DeviceContext> m_d2dDeviceContext;
};

} // namespace Microsoft::ReactNative::Composition
