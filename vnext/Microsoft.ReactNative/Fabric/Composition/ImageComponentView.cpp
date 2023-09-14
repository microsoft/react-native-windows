
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
#include <winrt/Windows.Graphics.Effects.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/Windows.Web.Http.h>
#include "Composition/AutoDraw.h"
#include "CompositionDynamicAutomationProvider.h"
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

void ImageComponentView::mountChildComponentView(IComponentView &childComponentView, uint32_t index) noexcept {
  assert(false);
}

void ImageComponentView::unmountChildComponentView(IComponentView &childComponentView, uint32_t index) noexcept {
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

  // TODO - handle imageProps.capInsets

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

  // update BaseComponentView props
  updateShadowProps(oldImageProps, newImageProps, m_visual);
  updateTransformProps(oldImageProps, newImageProps, m_visual);
  updateBorderProps(oldImageProps, newImageProps);

  if (oldImageProps.backgroundColor != newImageProps.backgroundColor ||
      oldImageProps.blurRadius != newImageProps.blurRadius || oldImageProps.tintColor != newImageProps.tintColor ||
      oldImageProps.resizeMode != newImageProps.resizeMode) {
    m_drawingSurface = nullptr; // TODO don't need to recreate the surface just to redraw...
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
    // Should ViewComponents enable_shared_from_this? then we don't need this dance to get a shared_ptr
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
    OuterVisual().IsVisible(layoutMetrics.displayType != facebook::react::DisplayType::None);
  }

  updateBorderLayoutMetrics(layoutMetrics, *m_props);

  m_layoutMetrics = layoutMetrics;

  UpdateCenterPropertySet();
  m_visual.Size(
      {layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor,
       layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor});
}

void ImageComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {
  if (m_needsBorderUpdate) {
    m_needsBorderUpdate = false;
    UpdateSpecialBorderLayers(m_layoutMetrics, *m_props);
  }
}

void ImageComponentView::OnRenderingDeviceLost() noexcept {
  // Draw the text again
  DrawImage();
}

void ImageComponentView::ensureDrawingSurface() noexcept {
  assert(m_context.UIDispatcher().HasThreadAccess());

  UINT width, height;
  winrt::check_hresult(m_wicbmp->GetSize(&width, &height));

  if (!m_drawingSurface && m_wicbmp) {
    winrt::Windows::Foundation::Size drawingSurfaceSize{static_cast<float>(width), static_cast<float>(height)};

    const auto imageProps = std::static_pointer_cast<const facebook::react::ImageProps>(m_props);
    const auto frame{m_layoutMetrics.getContentFrame().size};

    if (imageProps->resizeMode == facebook::react::ImageResizeMode::Repeat) {
      drawingSurfaceSize = {frame.width, frame.height};
    } else if (imageProps->blurRadius > 0) {
      // https://learn.microsoft.com/en-us/windows/win32/direct2d/gaussian-blur#output-bitmap
      // The following equation that can be used to compute the output bitmap:
      // Output bitmap growth (X and Y) = (StandardDeviation(DIPs)*3 + StandardDeviation(DIPs)*3)*((User DPI)/96)
      // Where StandardDeviation(DIPs)*3 is equivalent to the blur radius.
      const auto bmpGrowth{imageProps->blurRadius * 2 * m_layoutMetrics.pointScaleFactor};
      drawingSurfaceSize = {drawingSurfaceSize.Width + bmpGrowth, drawingSurfaceSize.Height + bmpGrowth};
    }

    m_drawingSurface = m_compContext.CreateDrawingSurfaceBrush(
        drawingSurfaceSize,
        winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
        winrt::Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied);

    DrawImage();

    switch (imageProps->resizeMode) {
      case facebook::react::ImageResizeMode::Stretch:
        m_drawingSurface.Stretch(winrt::Microsoft::ReactNative::Composition::CompositionStretch::Fill);
        break;
      case facebook::react::ImageResizeMode::Cover:
        m_drawingSurface.Stretch(winrt::Microsoft::ReactNative::Composition::CompositionStretch::UniformToFill);
        break;
      case facebook::react::ImageResizeMode::Contain:
        m_drawingSurface.Stretch(winrt::Microsoft::ReactNative::Composition::CompositionStretch::Uniform);
        break;
      case facebook::react::ImageResizeMode::Repeat:
        // TODO - set AlignmentRatio back to 0.5f when switching between resizeModes once we no longer recreate the
        // drawing surface on prop changes.
        m_drawingSurface.HorizontalAlignmentRatio(0.0f);
        m_drawingSurface.VerticalAlignmentRatio(0.0f);
        // Repeat and Center use the same Stretch logic, so we can fallthrough here.
        [[fallthrough]];
      case facebook::react::ImageResizeMode::Center: {
        m_drawingSurface.Stretch(
            (height < frame.height && width < frame.width)
                ? winrt::Microsoft::ReactNative::Composition::CompositionStretch::None
                : winrt::Microsoft::ReactNative::Composition::CompositionStretch::Uniform);
        break;
      }
      default:
        assert(false);
    }

    m_visual.Brush(m_drawingSurface);
  }
}

void ImageComponentView::DrawImage() noexcept {
  // Begin our update of the surface pixels. If this is our first update, we are required
  // to specify the entire surface, which nullptr is shorthand for (but, as it works out,
  // any time we make an update we touch the entire surface, so we always pass nullptr).
  POINT offset;

  assert(m_context.UIDispatcher().HasThreadAccess());

  ::Microsoft::ReactNative::Composition::AutoDrawDrawingSurface autoDraw(m_drawingSurface, &offset);
  if (auto d2dDeviceContext = autoDraw.GetRenderTarget()) {
    winrt::com_ptr<ID2D1Bitmap1> bitmap;
    winrt::check_hresult(d2dDeviceContext->CreateBitmapFromWicBitmap(m_wicbmp.get(), nullptr, bitmap.put()));

    d2dDeviceContext->Clear(D2D1::ColorF(D2D1::ColorF::Black, 0.0f));
    if (m_props->backgroundColor) {
      d2dDeviceContext->Clear(m_props->backgroundColor.AsD2DColor());
    }

    const auto imageProps = std::static_pointer_cast<const facebook::react::ImageProps>(m_props);

    bool useEffects{
        imageProps->blurRadius > 0 || isColorMeaningful(imageProps->tintColor) ||
        imageProps->resizeMode == facebook::react::ImageResizeMode::Repeat};

    if (useEffects) {
      winrt::com_ptr<ID2D1Effect> bitmapEffects;
      winrt::check_hresult(d2dDeviceContext->CreateEffect(CLSID_D2D1BitmapSource, bitmapEffects.put()));
      winrt::check_hresult(bitmapEffects->SetValue(D2D1_BITMAPSOURCE_PROP_WIC_BITMAP_SOURCE, m_wicbmp.get()));

      if (imageProps->blurRadius > 0) {
        winrt::com_ptr<ID2D1Effect> gaussianBlurEffect;
        winrt::check_hresult(d2dDeviceContext->CreateEffect(CLSID_D2D1GaussianBlur, gaussianBlurEffect.put()));
        // https://learn.microsoft.com/en-us/windows/win32/direct2d/gaussian-blur#effect-properties
        // You can compute the blur radius of the kernel by multiplying the standard deviation by 3 (radius multiplier).
        constexpr float radiusMultiplier = 3;
        winrt::check_hresult(gaussianBlurEffect->SetValue(
            D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, (imageProps->blurRadius) / radiusMultiplier));
        gaussianBlurEffect->SetInputEffect(0, bitmapEffects.get());
        bitmapEffects.copy_from(gaussianBlurEffect.get());
      }

      if (isColorMeaningful(imageProps->tintColor)) {
        winrt::com_ptr<ID2D1Effect> tintColorEffect;
        winrt::check_hresult(d2dDeviceContext->CreateEffect(CLSID_D2D1Flood, tintColorEffect.put()));
        winrt::check_hresult(tintColorEffect->SetValue(D2D1_FLOOD_PROP_COLOR, imageProps->tintColor.AsD2DColor()));

        winrt::com_ptr<ID2D1Effect> compositeEffect;
        winrt::check_hresult(d2dDeviceContext->CreateEffect(CLSID_D2D1Composite, compositeEffect.put()));
        winrt::check_hresult(compositeEffect->SetValue(D2D1_COMPOSITE_PROP_MODE, D2D1_COMPOSITE_MODE_SOURCE_IN));
        compositeEffect->SetInputEffect(0, bitmapEffects.get());
        compositeEffect->SetInputEffect(1, tintColorEffect.get());

        bitmapEffects.copy_from(compositeEffect.get());
      }

      if (imageProps->resizeMode == facebook::react::ImageResizeMode::Repeat) {
        winrt::com_ptr<ID2D1Effect> borderEffect;
        winrt::check_hresult(d2dDeviceContext->CreateEffect(CLSID_D2D1Border, borderEffect.put()));
        winrt::check_hresult(borderEffect->SetValue(D2D1_BORDER_PROP_EDGE_MODE_X, D2D1_BORDER_EDGE_MODE_WRAP));
        winrt::check_hresult(borderEffect->SetValue(D2D1_BORDER_PROP_EDGE_MODE_Y, D2D1_BORDER_EDGE_MODE_WRAP));
        borderEffect->SetInputEffect(0, bitmapEffects.get());

        d2dDeviceContext->DrawImage(borderEffect.get());
      } else {
        winrt::com_ptr<ID2D1Image> image;
        bitmapEffects->GetOutput(image.put());

        D2D1_RECT_F imageBounds;
        winrt::check_hresult(d2dDeviceContext->GetImageLocalBounds(image.get(), &imageBounds));

        d2dDeviceContext->DrawImage(
            bitmapEffects.get(), {static_cast<float>(offset.x), static_cast<float>(offset.y)}, imageBounds);
      }
    } else {
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
    }
  }
}

void ImageComponentView::prepareForRecycle() noexcept {}
facebook::react::Props::Shared ImageComponentView::props() noexcept {
  return m_props;
}

facebook::react::Tag ImageComponentView::hitTest(
    facebook::react::Point pt,
    facebook::react::Point &localPt,
    bool ignorePointerEvents) const noexcept {
  facebook::react::Point ptLocal{pt.x - m_layoutMetrics.frame.origin.x, pt.y - m_layoutMetrics.frame.origin.y};

  facebook::react::Tag targetTag;

  if ((ignorePointerEvents || m_props->pointerEvents == facebook::react::PointerEventsMode::Auto ||
       m_props->pointerEvents == facebook::react::PointerEventsMode::BoxNone) &&
      std::any_of(m_children.rbegin(), m_children.rend(), [&targetTag, &ptLocal, &localPt](auto child) {
        targetTag = static_cast<const CompositionBaseComponentView *>(child)->hitTest(ptLocal, localPt);
        return targetTag != -1;
      }))
    return targetTag;

  if ((ignorePointerEvents || m_props->pointerEvents == facebook::react::PointerEventsMode::Auto ||
       m_props->pointerEvents == facebook::react::PointerEventsMode::BoxOnly) &&
      ptLocal.x >= 0 && ptLocal.x <= m_layoutMetrics.frame.size.width && ptLocal.y >= 0 &&
      ptLocal.y <= m_layoutMetrics.frame.size.height) {
    localPt = ptLocal;
    return tag();
  }

  return -1;
}

void ImageComponentView::ensureVisual() noexcept {
  if (!m_visual) {
    m_visual = m_compContext.CreateSpriteVisual();
    OuterVisual().InsertAt(m_visual, 0);
  }
}

winrt::Microsoft::ReactNative::Composition::IVisual ImageComponentView::Visual() const noexcept {
  return m_visual;
}

bool ImageComponentView::focusable() const noexcept {
  return m_props->focusable;
}

std::string ImageComponentView::DefaultControlType() const noexcept {
  return "image";
}

std::shared_ptr<ImageComponentView> ImageComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  return std::shared_ptr<ImageComponentView>(new ImageComponentView(compContext, tag, reactContext));
}

} // namespace Microsoft::ReactNative
