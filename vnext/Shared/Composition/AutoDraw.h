// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <winrt/Microsoft.ReactNative.Composition.h>

namespace Microsoft::ReactNative::Composition {

class AutoDrawDrawingSurface {
 public:
  AutoDrawDrawingSurface(
      winrt::Microsoft::ReactNative::Composition::IDrawingSurfaceBrush &drawingSurface,
      POINT *offset) noexcept {
    drawingSurface.as(m_drawingSurfaceInterop);
    m_drawingSurfaceInterop->BeginDraw(m_d2dDeviceContext.put(), offset);
  }

  ~AutoDrawDrawingSurface() noexcept {
    if (m_d2dDeviceContext) {
      m_drawingSurfaceInterop->EndDraw();
    }
  }

  operator ID2D1DeviceContext *() noexcept {
    return m_d2dDeviceContext.get();
  }

  ID2D1DeviceContext *GetRenderTarget() noexcept {
    return m_d2dDeviceContext.get();
  }

 private:
  winrt::com_ptr<ICompositionDrawingSurfaceInterop> m_drawingSurfaceInterop;
  winrt::com_ptr<ID2D1DeviceContext> m_d2dDeviceContext;
};

} // namespace Microsoft::ReactNative::Composition
