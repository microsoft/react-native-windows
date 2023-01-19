
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ImageComponentView.h"

#include <UI.Xaml.Controls.h>
#include <Utils/ValueUtils.h>

#include <IReactContext.h>

#include <react/renderer/components/image/ImageEventEmitter.h>

#include <Fabric/FabricUIManagerModule.h>
#include <Utils/ImageUtils.h>
#include <shcore.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/Windows.Web.Http.h>
#include "CompositionHelpers.h"

extern "C" HRESULT WINAPI WICCreateImagingFactory_Proxy(UINT SDKVersion, IWICImagingFactory **ppIWICImagingFactory);

namespace Microsoft::ReactNative {

ImageComponentView::WindowsImageResponseObserver::WindowsImageResponseObserver(
    std::shared_ptr<ImageComponentView> image) {
  m_image = image;
}

void ImageComponentView::WindowsImageResponseObserver::didReceiveProgress(float progress) const {
  // TODO progress?
}

void ImageComponentView::WindowsImageResponseObserver::didReceiveImage(
    facebook::react::ImageResponse const &imageResponse) const {
  auto sharedwicbmp = std::static_pointer_cast<winrt::com_ptr<IWICBitmap>>(imageResponse.getImage());
  m_image->m_context.UIDispatcher().Post(
      [wicbmp = *sharedwicbmp, image = m_image]() { image->didReceiveImage(wicbmp); });
}

void ImageComponentView::WindowsImageResponseObserver::didReceiveFailure() const {
  m_image->didReceiveFailureFromObserver();
}

ImageComponentView::ImageComponentView(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : Super(compContext, tag), m_context(reactContext) {
  static auto const defaultProps = std::make_shared<facebook::react::ImageProps const>();
  m_props = defaultProps;
}

std::vector<facebook::react::ComponentDescriptorProvider>
ImageComponentView::supplementalComponentDescriptorProviders() noexcept {
  return {};
}

void ImageComponentView::mountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept {
  assert(false);
}

void ImageComponentView::unmountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept {
  assert(false);
}

void ImageComponentView::ImageLoadStart() noexcept {
  auto imageEventEmitter = std::static_pointer_cast<facebook::react::ImageEventEmitter const>(m_eventEmitter);
  if (imageEventEmitter) {
    imageEventEmitter->onLoadStart();
  }
}

void ImageComponentView::didReceiveImage(const winrt::com_ptr<IWICBitmap> &wicbmp) noexcept {
  // TODO check for recycled?

  auto imageEventEmitter = std::static_pointer_cast<facebook::react::ImageEventEmitter const>(m_eventEmitter);
  if (imageEventEmitter) {
    imageEventEmitter->onLoad();
    imageEventEmitter->onLoadEnd();
  }

  // TODO - handle m_props.tintColor, imageProps.resizeMode, imageProps.capInsets, imageProps.blurRadius

#ifdef DEBUG
  auto uiDispatcher = m_context.UIDispatcher();
  assert(uiDispatcher.HasThreadAccess());
#endif

  m_wicbmp = wicbmp;
  ensureDrawingSurface();
}

void ImageComponentView::didReceiveFailureFromObserver() noexcept {
  auto imageEventEmitter = std::static_pointer_cast<facebook::react::ImageEventEmitter const>(m_eventEmitter);
  if (imageEventEmitter) {
    imageEventEmitter->onError();
    imageEventEmitter->onLoadEnd();
  }
}

void ImageComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldImageProps = *std::static_pointer_cast<const facebook::react::ImageProps>(m_props);
  const auto &newImageProps = *std::static_pointer_cast<const facebook::react::ImageProps>(props);

  ensureVisual();

  updateBorderProps(oldImageProps, newImageProps);

  if (oldImageProps.backgroundColor != newImageProps.backgroundColor) {
    m_drawingSurface = nullptr; // TODO dont need to recreate the surface just to redraw...
  }

  if (oldImageProps.opacity != newImageProps.opacity) {
    m_visual.Opacity(newImageProps.opacity);
  }

  m_props = std::static_pointer_cast<facebook::react::ImageProps const>(props);
}

void ImageComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {
  auto oldImageState = std::static_pointer_cast<facebook::react::ImageShadowNode::ConcreteState const>(m_state);
  auto newImageState = std::static_pointer_cast<facebook::react::ImageShadowNode::ConcreteState const>(state);

  if (!m_imageResponseObserver) {
    // Should ViewComponents enable_shared_from_this? then we dont need this dance to get a shared_ptr
    std::shared_ptr<FabricUIManager> fabricuiManager =
        ::Microsoft::ReactNative::FabricUIManager::FromProperties(m_context.Properties());
    auto componentViewDescriptor = fabricuiManager->GetViewRegistry().componentViewDescriptorWithTag(m_tag);

    m_imageResponseObserver = std::make_shared<WindowsImageResponseObserver>(
        std::static_pointer_cast<ImageComponentView>(componentViewDescriptor.view));
  }

  setStateAndResubscribeImageResponseObserver(newImageState);
  bool havePreviousData = oldImageState && oldImageState->getData().getImageSource() != facebook::react::ImageSource{};

  if (!havePreviousData ||
      (newImageState && newImageState->getData().getImageSource() != oldImageState->getData().getImageSource())) {
    // Loading actually starts a little before this, but this is the first time we know
    // the image is loading and can fire an event from this component
    std::static_pointer_cast<facebook::react::ImageEventEmitter const>(m_eventEmitter)->onLoadStart();
  }
}

void ImageComponentView::setStateAndResubscribeImageResponseObserver(
    facebook::react::ImageShadowNode::ConcreteState::Shared const &state) noexcept {
  if (m_state) {
    auto &observerCoordinator = m_state->getData().getImageRequest().getObserverCoordinator();
    observerCoordinator.removeObserver(*m_imageResponseObserver);
  }

  m_state = state;

  if (m_state) {
    auto &observerCoordinator = m_state->getData().getImageRequest().getObserverCoordinator();
    observerCoordinator.addObserver(*m_imageResponseObserver);
  }
}

void ImageComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  // Set Position & Size Properties

  if ((layoutMetrics.displayType != m_layoutMetrics.displayType)) {
    m_visual.IsVisible(layoutMetrics.displayType != facebook::react::DisplayType::None);
  }

  updateBorderLayoutMetrics(layoutMetrics, *m_props);

  m_layoutMetrics = layoutMetrics;

  UpdateCenterPropertySet();
  m_visual.Size(
      {layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor,
       layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor});
  m_visual.Offset({
      layoutMetrics.frame.origin.x * layoutMetrics.pointScaleFactor,
      layoutMetrics.frame.origin.y * layoutMetrics.pointScaleFactor,
      0.0f,
  });
}

void ImageComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {}

void ImageComponentView::OnRenderingDeviceLost() noexcept {
  // Draw the text again
  DrawImage();
}

void ImageComponentView::ensureDrawingSurface() noexcept {
  assert(m_context.UIDispatcher().HasThreadAccess());

  UINT width, height;
  winrt::check_hresult(m_wicbmp->GetSize(&width, &height));

  if (!m_drawingSurface && m_wicbmp) {
    m_drawingSurface = m_compContext.CreateDrawingSurface(
        {static_cast<float>(width), static_cast<float>(height)},
        winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
        winrt::Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied);

    DrawImage();

    auto surfaceBrush = m_compContext.CreateSurfaceBrush(m_drawingSurface);

    const auto &imageProps = *std::static_pointer_cast<const facebook::react::ImageProps>(m_props);
    switch (imageProps.resizeMode) {
      case facebook::react::ImageResizeMode::Stretch:
        surfaceBrush.Stretch(winrt::Microsoft::ReactNative::Composition::CompositionStretch::Fill);
        break;
      case facebook::react::ImageResizeMode::Cover:
        surfaceBrush.Stretch(winrt::Microsoft::ReactNative::Composition::CompositionStretch::UniformToFill);
        break;
      case facebook::react::ImageResizeMode::Contain:
        surfaceBrush.Stretch(winrt::Microsoft::ReactNative::Composition::CompositionStretch::Uniform);
        break;
      case facebook::react::ImageResizeMode::Center:
        surfaceBrush.Stretch(winrt::Microsoft::ReactNative::Composition::CompositionStretch::None);
        break;
      case facebook::react::ImageResizeMode::Repeat:
        surfaceBrush.Stretch(winrt::Microsoft::ReactNative::Composition::CompositionStretch::UniformToFill);
        // TODO - Hook up repeat
        break;
      default:
        assert(false);
    }
    m_visual.Brush(surfaceBrush);
  }
}

void ImageComponentView::DrawImage() noexcept {
  // Begin our update of the surface pixels. If this is our first update, we are required
  // to specify the entire surface, which nullptr is shorthand for (but, as it works out,
  // any time we make an update we touch the entire surface, so we always pass nullptr).
  winrt::com_ptr<ID2D1DeviceContext> d2dDeviceContext;
  POINT offset;

  assert(m_context.UIDispatcher().HasThreadAccess());

  winrt::com_ptr<Composition::ICompositionDrawingSurfaceInterop> drawingSurfaceInterop;
  m_drawingSurface.as(drawingSurfaceInterop);

  if (CheckForDeviceRemoved(drawingSurfaceInterop->BeginDraw(d2dDeviceContext.put(), &offset))) {
    const auto &paragraphProps = *std::static_pointer_cast<const facebook::react::ImageProps>(m_props);

    winrt::com_ptr<ID2D1Bitmap1> bitmap;
    winrt::check_hresult(d2dDeviceContext->CreateBitmapFromWicBitmap(m_wicbmp.get(), nullptr, bitmap.put()));

    d2dDeviceContext->Clear(D2D1::ColorF(D2D1::ColorF::Black, 0.0f));
    if (m_props->backgroundColor) {
      d2dDeviceContext->Clear(m_props->backgroundColor.AsD2DColor());
    }

    UINT width, height;
    winrt::check_hresult(m_wicbmp->GetSize(&width, &height));

    D2D1_RECT_F rect = D2D1::RectF(
        static_cast<float>(offset.x / m_layoutMetrics.pointScaleFactor),
        static_cast<float>(offset.y / m_layoutMetrics.pointScaleFactor),
        static_cast<float>((offset.x + width) / m_layoutMetrics.pointScaleFactor),
        static_cast<float>((offset.y + height) / m_layoutMetrics.pointScaleFactor));

    const auto dpi = m_layoutMetrics.pointScaleFactor * 96.0f;
    float oldDpiX, oldDpiY;
    d2dDeviceContext->GetDpi(&oldDpiX, &oldDpiY);
    d2dDeviceContext->SetDpi(dpi, dpi);

    d2dDeviceContext->DrawBitmap(bitmap.get(), rect);

    // Restore old dpi setting
    d2dDeviceContext->SetDpi(oldDpiX, oldDpiY);

    // Our update is done. EndDraw never indicates rendering device removed, so any
    // failure here is unexpected and, therefore, fatal.
    winrt::check_hresult(drawingSurfaceInterop->EndDraw());
  }
}

void ImageComponentView::prepareForRecycle() noexcept {}
facebook::react::Props::Shared ImageComponentView::props() noexcept {
  return m_props;
}

facebook::react::Tag ImageComponentView::hitTest(facebook::react::Point pt, facebook::react::Point &localPt)
    const noexcept {
  facebook::react::Point ptLocal{pt.x - m_layoutMetrics.frame.origin.x, pt.y - m_layoutMetrics.frame.origin.y};

  facebook::react::Tag targetTag;

  if ((m_props->pointerEvents == facebook::react::PointerEventsMode::Auto ||
       m_props->pointerEvents == facebook::react::PointerEventsMode::BoxNone) &&
      std::any_of(m_children.rbegin(), m_children.rend(), [&targetTag, &ptLocal, &localPt](auto child) {
        targetTag = static_cast<const CompositionBaseComponentView *>(child)->hitTest(ptLocal, localPt);
        return targetTag != -1;
      }))
    return targetTag;

  if ((m_props->pointerEvents == facebook::react::PointerEventsMode::Auto ||
       m_props->pointerEvents == facebook::react::PointerEventsMode::BoxOnly) &&
      ptLocal.x >= 0 && ptLocal.x <= m_layoutMetrics.frame.size.width && ptLocal.y >= 0 &&
      ptLocal.y <= m_layoutMetrics.frame.size.height) {
    localPt = ptLocal;
    return tag();
  }

  return -1;
}

facebook::react::SharedTouchEventEmitter ImageComponentView::touchEventEmitter() noexcept {
  return m_eventEmitter;
}

void ImageComponentView::ensureVisual() noexcept {
  if (!m_visual) {
    m_visual = m_compContext.CreateSpriteVisual();
  }
}

winrt::Microsoft::ReactNative::Composition::IVisual ImageComponentView::Visual() const noexcept {
  return m_visual;
}

} // namespace Microsoft::ReactNative
