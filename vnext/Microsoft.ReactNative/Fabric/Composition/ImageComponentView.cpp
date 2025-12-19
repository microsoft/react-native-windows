
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ImageComponentView.h"

#include <Utils/ValueUtils.h>

#include <IReactContext.h>

#include <react/renderer/components/image/ImageEventEmitter.h>

#include <AutoDraw.h>
#include <Fabric/AbiViewProps.h>
#include <Fabric/FabricUIManagerModule.h>
#include <Utils/ImageUtils.h>
#include <shcore.h>
#include <winrt/Windows.Graphics.Effects.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/Windows.Web.Http.h>
#include "CompositionDynamicAutomationProvider.h"
#include "CompositionHelpers.h"
#include "RootComponentView.h"

extern "C" HRESULT WINAPI WICCreateImagingFactory_Proxy(UINT SDKVersion, IWICImagingFactory **ppIWICImagingFactory);

namespace winrt::Microsoft::ReactNative::Composition::implementation {

ImageComponentView::WindowsImageResponseObserver::WindowsImageResponseObserver(
    winrt::Microsoft::ReactNative::ReactContext const &reactContext,
    winrt::weak_ref<winrt::Microsoft::ReactNative::Composition::implementation::ImageComponentView> wkImage)
    : m_reactContext(reactContext), m_wkImage(std::move(wkImage)) {}

void ImageComponentView::WindowsImageResponseObserver::didReceiveProgress(float progress, int64_t loaded, int64_t total)
    const {
  int loadedInt = static_cast<int>(loaded);
  int totalInt = static_cast<int>(total);
  m_reactContext.UIDispatcher().Post([progress, wkImage = m_wkImage, loadedInt, totalInt]() {
    if (auto image = wkImage.get()) {
      image->didReceiveProgress(progress, loadedInt, totalInt);
    }
  });
}

void ImageComponentView::WindowsImageResponseObserver::didReceiveImage(
    facebook::react::ImageResponse const &imageResponse) const {
  auto imageResponseImage = std::static_pointer_cast<ImageResponseImage>(imageResponse.getImage());
  m_reactContext.UIDispatcher().Post([imageResponseImage, wkImage = m_wkImage]() {
    if (auto image{wkImage.get()}) {
      image->didReceiveImage(imageResponseImage);
    }
  });
}

void ImageComponentView::WindowsImageResponseObserver::didReceiveFailure(
    facebook::react::ImageLoadError const &error) const {
  if (auto imgComponentView{m_wkImage.get()}) {
    imgComponentView->didReceiveFailureFromObserver(error);
  }
}

facebook::react::SharedViewProps ImageComponentView::defaultProps() noexcept {
  static auto const defaultProps = std::make_shared<facebook::react::ImageProps const>();
  return defaultProps;
}

ImageComponentView::ImageComponentView(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : Super(
          ImageComponentView::defaultProps(),
          compContext,
          tag,
          reactContext,
          ComponentViewFeatures::Default & ~ComponentViewFeatures::Background) {}

void ImageComponentView::MountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
  base_type::MountChildComponentView(childComponentView, index);
}

void ImageComponentView::UnmountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
  base_type::UnmountChildComponentView(childComponentView, index);
}

void ImageComponentView::ImageLoadStart() noexcept {
  auto imageEventEmitter = std::static_pointer_cast<facebook::react::ImageEventEmitter const>(m_eventEmitter);
  if (imageEventEmitter) {
    imageEventEmitter->onLoadStart();
  }
}

void ImageComponentView::ImageLoaded() noexcept {
  auto imageEventEmitter = std::static_pointer_cast<facebook::react::ImageEventEmitter const>(m_eventEmitter);
  if (imageEventEmitter) {
    imageEventEmitter->onLoadEnd();
  }
}

void ImageComponentView::didReceiveProgress(float progress, int loaded, int total) noexcept {
  auto imageEventEmitter = std::static_pointer_cast<facebook::react::ImageEventEmitter const>(m_eventEmitter);
  if (imageEventEmitter) {
    imageEventEmitter->onProgress(progress, loaded, total);
  }
  ensureDrawingSurface();
}

void ImageComponentView::didReceiveImage(const std::shared_ptr<ImageResponseImage> &imageResponseImage) noexcept {
  // TODO check for recycled?

  auto imageEventEmitter = std::static_pointer_cast<facebook::react::ImageEventEmitter const>(m_eventEmitter);
  if (imageEventEmitter) {
    imageEventEmitter->onLoad(m_state->getData().getImageSource());
    imageEventEmitter->onLoadEnd();
  }

  // TODO - handle imageProps.capInsets

#ifdef DEBUG
  auto uiDispatcher = m_reactContext.UIDispatcher();
  assert(uiDispatcher.HasThreadAccess());
#endif

  m_imageResponseImage = imageResponseImage;
  ensureDrawingSurface();
}

void ImageComponentView::didReceiveFailureFromObserver(const facebook::react::ImageLoadError &error) noexcept {
  auto imageEventEmitter = std::static_pointer_cast<facebook::react::ImageEventEmitter const>(m_eventEmitter);
  if (imageEventEmitter) {
    std::shared_ptr<facebook::react::ImageErrorInfo> errorInfo =
        std::static_pointer_cast<facebook::react::ImageErrorInfo>(error.getError());
    imageEventEmitter->onError(*errorInfo);
    imageEventEmitter->onLoadEnd();
  }
}

const facebook::react::ImageProps &ImageComponentView::imageProps() const noexcept {
  return *std::static_pointer_cast<const facebook::react::ImageProps>(viewProps());
}

void ImageComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldImageProps =
      *std::static_pointer_cast<const facebook::react::ImageProps>(oldProps ? oldProps : viewProps());
  const auto &newImageProps = *std::static_pointer_cast<const facebook::react::ImageProps>(props);

  if (oldImageProps.backgroundColor != newImageProps.backgroundColor ||
      oldImageProps.blurRadius != newImageProps.blurRadius || oldImageProps.tintColor != newImageProps.tintColor ||
      oldImageProps.resizeMode != newImageProps.resizeMode) {
    m_drawingSurface = nullptr; // TODO don't need to recreate the surface just to redraw...
  }

  Super::updateProps(props, oldProps);
}

void ImageComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {
  auto oldImageState = std::static_pointer_cast<facebook::react::ImageShadowNode::ConcreteState const>(m_state);
  auto newImageState = std::static_pointer_cast<facebook::react::ImageShadowNode::ConcreteState const>(state);

  if (!m_imageResponseObserver) {
    m_imageResponseObserver = std::make_shared<WindowsImageResponseObserver>(this->m_reactContext, get_weak());
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

void ImageComponentView::prepareForRecycle() noexcept {
  setStateAndResubscribeImageResponseObserver(nullptr);
}

winrt::Microsoft::ReactNative::ImageProps ImageComponentView::ImageProps() noexcept {
  // We do not currently support custom ImageComponentView's
  // If we did we would need to create a AbiImageProps and possibly return them here
  return winrt::make<winrt::Microsoft::ReactNative::implementation::ImageProps>(viewProps());
}

winrt::Microsoft::ReactNative::ImageProps ImageComponentView::ViewProps() noexcept {
  return ViewPropsInner().as<winrt::Microsoft::ReactNative::ImageProps>();
}

winrt::Microsoft::ReactNative::ViewProps ImageComponentView::ViewPropsInner() noexcept {
  // We do not currently support custom ImageComponentView's
  // If we did we would need to create a AbiImageProps and possibly return them here
  return winrt::make<winrt::Microsoft::ReactNative::implementation::ImageProps>(viewProps());
}

void ImageComponentView::OnRenderingDeviceLost() noexcept {
  m_drawingSurface = nullptr;
  ensureDrawingSurface();
}

bool ImageComponentView::themeEffectsImage() const noexcept {
  return viewProps()->backgroundColor || isColorMeaningful(imageProps().tintColor);
}

void ImageComponentView::onThemeChanged() noexcept {
  if (themeEffectsImage()) {
    m_drawingSurface = nullptr;
    ensureDrawingSurface();
  }
  Super::onThemeChanged();
}

void ImageComponentView::ensureDrawingSurface() noexcept {
  assert(m_reactContext.UIDispatcher().HasThreadAccess());

  if (!m_imageResponseImage) {
    Visual().as<Experimental::ISpriteVisual>().Brush(nullptr);
    return;
  }

  UINT width = 0, height = 0;
  if (m_imageResponseImage->m_wicbmp) {
    winrt::check_hresult(m_imageResponseImage->m_wicbmp->GetSize(&width, &height));
  }

  if (!m_drawingSurface && m_imageResponseImage->m_wicbmp) {
    winrt::Windows::Foundation::Size drawingSurfaceSize{static_cast<float>(width), static_cast<float>(height)};

    const auto &imgProps = imageProps();
    const auto frame{m_layoutMetrics.getContentFrame().size};

    if (imgProps.resizeMode == facebook::react::ImageResizeMode::Repeat) {
      drawingSurfaceSize = {
          frame.width * m_layoutMetrics.pointScaleFactor, frame.height * m_layoutMetrics.pointScaleFactor};
    } else if (imgProps.blurRadius > 0) {
      // https://learn.microsoft.com/en-us/windows/win32/direct2d/gaussian-blur#output-bitmap
      // The following equation that can be used to compute the output bitmap:
      // Output bitmap growth (X and Y) = (StandardDeviation(DIPs)*3 + StandardDeviation(DIPs)*3)*((User DPI)/96)
      // Where StandardDeviation(DIPs)*3 is equivalent to the blur radius.
      const auto bmpGrowth{imgProps.blurRadius * 2 * m_layoutMetrics.pointScaleFactor};
      drawingSurfaceSize = {drawingSurfaceSize.Width + bmpGrowth, drawingSurfaceSize.Height + bmpGrowth};
    }

    m_drawingSurface = m_compContext.CreateDrawingSurfaceBrush(
        drawingSurfaceSize,
        winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
        winrt::Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied);

    DrawImage();

    switch (imgProps.resizeMode) {
      case facebook::react::ImageResizeMode::Stretch:
        m_drawingSurface.Stretch(winrt::Microsoft::ReactNative::Composition::Experimental::CompositionStretch::Fill);
        break;
      case facebook::react::ImageResizeMode::Cover:
        m_drawingSurface.Stretch(
            winrt::Microsoft::ReactNative::Composition::Experimental::CompositionStretch::UniformToFill);
        break;
      case facebook::react::ImageResizeMode::Contain:
        m_drawingSurface.Stretch(winrt::Microsoft::ReactNative::Composition::Experimental::CompositionStretch::Uniform);
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
                ? winrt::Microsoft::ReactNative::Composition::Experimental::CompositionStretch::None
                : winrt::Microsoft::ReactNative::Composition::Experimental::CompositionStretch::Uniform);
        break;
      }
      case facebook::react::ImageResizeMode::None:
        m_drawingSurface.Stretch(winrt::Microsoft::ReactNative::Composition::Experimental::CompositionStretch::None);
        break;
      default:
        assert(false);
    }

    Visual().as<Experimental::ISpriteVisual>().Brush(m_drawingSurface);
  } else if (m_imageResponseImage->m_brushFactory) {
    Visual().as<Experimental::ISpriteVisual>().Brush(
        m_imageResponseImage->m_brushFactory(m_reactContext.Handle(), m_compContext));
  }
}

void ImageComponentView::DrawImage() noexcept {
  // Begin our update of the surface pixels. If this is our first update, we are required
  // to specify the entire surface, which nullptr is shorthand for (but, as it works out,
  // any time we make an update we touch the entire surface, so we always pass nullptr).
  POINT offset;

  if (themeEffectsImage() && theme()->IsEmpty()) {
    return;
  }

  if (!m_imageResponseImage->m_wicbmp) {
    return;
  }

  assert(m_reactContext.UIDispatcher().HasThreadAccess());

  ::Microsoft::ReactNative::Composition::AutoDrawDrawingSurface autoDraw(m_drawingSurface, 1.0f, &offset);
  if (auto d2dDeviceContext = autoDraw.GetRenderTarget()) {
    winrt::com_ptr<ID2D1Bitmap1> bitmap;
    winrt::check_hresult(
        d2dDeviceContext->CreateBitmapFromWicBitmap(m_imageResponseImage->m_wicbmp.get(), nullptr, bitmap.put()));

    d2dDeviceContext->Clear(D2D1::ColorF(D2D1::ColorF::Black, 0.0f));
    if (viewProps()->backgroundColor) {
      d2dDeviceContext->Clear(theme()->D2DColor(*viewProps()->backgroundColor));
    }

    const auto &imgProps = imageProps();

    bool useEffects{
        imgProps.blurRadius > 0 || isColorMeaningful(imgProps.tintColor) ||
        imgProps.resizeMode == facebook::react::ImageResizeMode::Repeat};

    if (useEffects) {
      winrt::com_ptr<ID2D1Effect> bitmapEffects;
      winrt::check_hresult(d2dDeviceContext->CreateEffect(CLSID_D2D1BitmapSource, bitmapEffects.put()));
      winrt::check_hresult(
          bitmapEffects->SetValue(D2D1_BITMAPSOURCE_PROP_WIC_BITMAP_SOURCE, m_imageResponseImage->m_wicbmp.get()));

      if (imgProps.blurRadius > 0) {
        winrt::com_ptr<ID2D1Effect> gaussianBlurEffect;
        winrt::check_hresult(d2dDeviceContext->CreateEffect(CLSID_D2D1GaussianBlur, gaussianBlurEffect.put()));
        // https://learn.microsoft.com/en-us/windows/win32/direct2d/gaussian-blur#effect-properties
        // You can compute the blur radius of the kernel by multiplying the standard deviation by 3 (radius multiplier).
        constexpr float radiusMultiplier = 3;
        winrt::check_hresult(gaussianBlurEffect->SetValue(
            D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, (imgProps.blurRadius) / radiusMultiplier));
        gaussianBlurEffect->SetInputEffect(0, bitmapEffects.get());
        bitmapEffects.copy_from(gaussianBlurEffect.get());
      }

      if (isColorMeaningful(imgProps.tintColor)) {
        winrt::com_ptr<ID2D1Effect> tintColorEffect;
        winrt::check_hresult(d2dDeviceContext->CreateEffect(CLSID_D2D1Flood, tintColorEffect.put()));
        winrt::check_hresult(tintColorEffect->SetValue(D2D1_FLOOD_PROP_COLOR, theme()->D2DColor(*imgProps.tintColor)));

        winrt::com_ptr<ID2D1Effect> compositeEffect;
        winrt::check_hresult(d2dDeviceContext->CreateEffect(CLSID_D2D1Composite, compositeEffect.put()));
        winrt::check_hresult(compositeEffect->SetValue(D2D1_COMPOSITE_PROP_MODE, D2D1_COMPOSITE_MODE_SOURCE_IN));
        compositeEffect->SetInputEffect(0, bitmapEffects.get());
        compositeEffect->SetInputEffect(1, tintColorEffect.get());

        bitmapEffects.copy_from(compositeEffect.get());
      }

      if (imgProps.resizeMode == facebook::react::ImageResizeMode::Repeat) {
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
      winrt::check_hresult(m_imageResponseImage->m_wicbmp->GetSize(&width, &height));

      D2D1_RECT_F rect = D2D1::RectF(
          static_cast<float>(offset.x),
          static_cast<float>(offset.y),
          static_cast<float>(offset.x + width),
          static_cast<float>(offset.y + height));

      d2dDeviceContext->DrawBitmap(bitmap.get(), rect);
    }
  }
}

std::string ImageComponentView::DefaultControlType() const noexcept {
  return "image";
}

winrt::Microsoft::ReactNative::ComponentView ImageComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  return winrt::make<ImageComponentView>(compContext, tag, reactContext);
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
