
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ImageComponentView.h"

#include <UI.Xaml.Controls.h>
#include <Utils/ValueUtils.h>

#include <IReactContext.h>

#pragma warning(push)
#pragma warning(disable : 4244 4305)
#include <react/renderer/components/image/ImageProps.h>
#pragma warning(pop)
#include <react/renderer/components/image/ImageEventEmitter.h>

#include <Utils/ImageUtils.h>
#include <shcore.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/Windows.Web.Http.h>
#include "CompositionHelpers.h"

extern "C" HRESULT WINAPI WICCreateImagingFactory_Proxy(UINT SDKVersion, IWICImagingFactory **ppIWICImagingFactory);

namespace Microsoft::ReactNative {

ImageComponentView::ImageComponentView(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : Super(compContext, tag), m_context(reactContext) {
  static auto const defaultProps = std::make_shared<facebook::react::ImageProps const>();
  m_props = defaultProps;

  /*
    m_onLoadEndToken = m_element->OnLoadEnd([=](const auto &, const bool &succeeded) {
      if (succeeded) {
        std::static_pointer_cast<const facebook::react::ImageEventEmitter>(m_eventEmitter)->onLoad();
      } else {
        std::static_pointer_cast<const facebook::react::ImageEventEmitter>(m_eventEmitter)->onError();
      }
      std::static_pointer_cast<const facebook::react::ImageEventEmitter>(m_eventEmitter)->onLoadEnd();
    });
    */
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

void ImageComponentView::beginDownloadImage() noexcept {
  ReactImageSource source;
  source.uri = m_url;
  source.height = m_imgHeight;
  source.width = m_imgWidth;
  source.sourceType = ImageSourceType::Download;
  m_state = ImageState::Loading;
  auto inputStreamTask = GetImageStreamAsync(source);
  inputStreamTask.Completed([this](auto asyncOp, auto status) {
    switch (status) {
      case winrt::Windows::Foundation::AsyncStatus::Completed: {
        if (m_state == ImageState::Loading) {
          m_state = ImageState::Loaded;
          generateBitmap(asyncOp.GetResults());
        }
        break;
      }
      case winrt::Windows::Foundation::AsyncStatus::Canceled: {
        m_state = ImageState::Error;
        break;
      }
      case winrt::Windows::Foundation::AsyncStatus::Error: {
        m_state = ImageState::Error;
        break;
      }
      case winrt::Windows::Foundation::AsyncStatus::Started: {
      }
    }
  });
}

winrt::com_ptr<IWICBitmapSource> wicBitmapSourceFromStream(
    const winrt::Windows::Storage::Streams::IRandomAccessStream &results) noexcept {
  winrt::com_ptr<IWICBitmapDecoder> bitmapDecoder;
  winrt::com_ptr<IWICImagingFactory> imagingFactory;
  winrt::check_hresult(WICCreateImagingFactory_Proxy(WINCODEC_SDK_VERSION, imagingFactory.put()));

  if (!results) {
    return nullptr;
  }

  winrt::com_ptr<IStream> istream;
  winrt::check_hresult(
      CreateStreamOverRandomAccessStream(results.as<IUnknown>().get(), __uuidof(IStream), istream.put_void()));

  if (imagingFactory->CreateDecoderFromStream(
          istream.get(), nullptr, WICDecodeMetadataCacheOnDemand, bitmapDecoder.put()) < 0) {
    return nullptr;
  }

  winrt::com_ptr<IWICBitmapFrameDecode> decodedFrame;
  winrt::check_hresult(bitmapDecoder->GetFrame(0, decodedFrame.put()));
  return decodedFrame;
}

void ImageComponentView::generateBitmap(const winrt::Windows::Storage::Streams::IRandomAccessStream &results) noexcept {
  winrt::com_ptr<IWICBitmapSource> decodedFrame = wicBitmapSourceFromStream(results);

  if (!decodedFrame) {
    m_state = ImageState::Error;
    return;
  }

  winrt::com_ptr<IWICImagingFactory> imagingFactory;
  winrt::check_hresult(WICCreateImagingFactory_Proxy(WINCODEC_SDK_VERSION, imagingFactory.put()));
  winrt::com_ptr<IWICFormatConverter> converter;
  winrt::check_hresult(imagingFactory->CreateFormatConverter(converter.put()));

  winrt::check_hresult(converter->Initialize(
      decodedFrame.get(),
      GUID_WICPixelFormat32bppPBGRA,
      WICBitmapDitherTypeNone,
      nullptr,
      0.0f,
      WICBitmapPaletteTypeMedianCut));

  winrt::check_hresult(imagingFactory->CreateBitmapFromSource(converter.get(), WICBitmapCacheOnLoad, m_wicbmp.put()));

  auto uiDispatcher = m_context.UIDispatcher();
  if (uiDispatcher.HasThreadAccess()) {
    ensureDrawingSurface();
  } else {
    uiDispatcher.Post([this]() { ensureDrawingSurface(); }); // TODO check capture ref
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

  if (oldImageProps.sources != newImageProps.sources) {
    if (newImageProps.sources.empty()) {
      // TODO clear image
    } else {
      m_reloadImage = true;
    }
  }

  /*
    if (oldImageProps.blurRadius != newImageProps.blurRadius) {
      m_element->BlurRadius(newImageProps.blurRadius);
    }

    if (oldImageProps.tintColor != newImageProps.tintColor) {
      if (newImageProps.tintColor) {
        m_element->TintColor(newImageProps.tintColor.AsWindowsColor());
      } else {
        m_element->TintColor(winrt::Colors::Transparent());
      }
    }

    if (oldImageProps.resizeMode != newImageProps.resizeMode) {
      m_element->ResizeMode(newImageProps.resizeMode);
    }
    */

  m_props = std::static_pointer_cast<facebook::react::ImageProps const>(props);
}

void ImageComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {}

void ImageComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  // Set Position & Size Properties

  if ((layoutMetrics.displayType != m_layoutMetrics.displayType)) {
    m_visual.IsVisible(layoutMetrics.displayType != facebook::react::DisplayType::None);
  }

  updateBorderLayoutMetrics(layoutMetrics, *m_props);

  m_layoutMetrics = layoutMetrics;

  m_visual.Size(
      {layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor,
       layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor});
  m_visual.Offset({
      layoutMetrics.frame.origin.x * layoutMetrics.pointScaleFactor,
      layoutMetrics.frame.origin.y * layoutMetrics.pointScaleFactor,
      0.0f,
  });
}

void ImageComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {
  if (m_reloadImage) {
    const auto &props = *std::static_pointer_cast<const facebook::react::ImageProps>(m_props);
    m_url = props.sources[0].uri;
    m_imgWidth = static_cast<unsigned int>(std::max(props.sources[0].size.width, m_layoutMetrics.frame.size.width));
    m_imgHeight = static_cast<unsigned int>(std::max(props.sources[0].size.height, m_layoutMetrics.frame.size.height));
    beginDownloadImage();
  }

  /*
  if (m_needsOnLoadStart) {
    std::static_pointer_cast<const facebook::react::ImageEventEmitter>(m_eventEmitter)->onLoadStart();
    m_needsOnLoadStart = false;
  }
  */

  ensureDrawingSurface();
}

void ImageComponentView::OnRenderingDeviceLost() noexcept {
  // Draw the text again
  DrawImage();
}

void ImageComponentView::ensureDrawingSurface() noexcept {
  assert(m_context.UIDispatcher().HasThreadAccess());

  if (!m_drawingSurface && m_wicbmp) {
    m_drawingSurface = m_compContext.CreateDrawingSurface(
        {static_cast<float>(m_imgWidth), static_cast<float>(m_imgHeight)},
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

    D2D1_RECT_F rect = D2D1::RectF(
        static_cast<float>(offset.x / m_layoutMetrics.pointScaleFactor),
        static_cast<float>(offset.y / m_layoutMetrics.pointScaleFactor),
        static_cast<float>((offset.x + m_imgWidth) / m_layoutMetrics.pointScaleFactor),
        static_cast<float>((offset.y + m_imgHeight) / m_layoutMetrics.pointScaleFactor));

    d2dDeviceContext->DrawBitmap(bitmap.get(), rect);

    // Our update is done. EndDraw never indicates rendering device removed, so any
    // failure here is unexpected and, therefore, fatal.
    winrt::check_hresult(drawingSurfaceInterop->EndDraw());
  }
}

void ImageComponentView::prepareForRecycle() noexcept {}
facebook::react::Props::Shared ImageComponentView::props() noexcept {
  assert(false);
  return {};
}

facebook::react::Tag ImageComponentView::hitTest(facebook::react::Point pt, facebook::react::Point &localPt)
    const noexcept {
  facebook::react::Point ptLocal{pt.x - m_layoutMetrics.frame.origin.x, pt.y - m_layoutMetrics.frame.origin.y};

  /*
    facebook::react::Tag tag;
    if (std::any_of(m_children.rbegin(), m_children.rend(), [&tag, &ptLocal, &localPt](auto child) {
          tag = static_cast<const CompositionBaseComponentView *>(child)->hitTest(ptLocal, localPt);
          return tag != -1;
        }))
      return tag;
  */
  if (ptLocal.x >= 0 && ptLocal.x <= m_layoutMetrics.frame.size.width && ptLocal.y >= 0 &&
      ptLocal.y <= m_layoutMetrics.frame.size.height) {
    localPt = ptLocal;
    return Tag();
  }

  return -1;
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
