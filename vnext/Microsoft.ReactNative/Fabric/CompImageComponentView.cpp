
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "CompImageComponentView.h"

#include <UI.Xaml.Controls.h>
#include <Utils/ValueUtils.h>

#include <IReactContext.h>

#pragma warning(push)
#pragma warning(disable : 4244 4305)
#include <react/renderer/components/image/ImageProps.h>
#pragma warning(pop)
#include <react/renderer/components/image/ImageEventEmitter.h>

#include <shcore.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.Web.Http.h>
#include "CompHelpers.h"

extern "C" HRESULT WINAPI WICCreateImagingFactory_Proxy(UINT SDKVersion, IWICImagingFactory **ppIWICImagingFactory);

namespace Microsoft::ReactNative {

CompImageComponentView::CompImageComponentView(winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : m_context(reactContext) {
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
CompImageComponentView::supplementalComponentDescriptorProviders() noexcept {
  return {};
}

void CompImageComponentView::mountChildComponentView(
    const IComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
}

void CompImageComponentView::unmountChildComponentView(
    const IComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
}

winrt::IAsyncOperation<winrt::Windows::Storage::Streams::InMemoryRandomAccessStream> GetImageMemoryStreamAsync(
    ReactImageSource source);

void CompImageComponentView::beginDownloadImage() noexcept {
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

void CompImageComponentView::generateBitmap(
    const winrt::Windows::Storage::Streams::InMemoryRandomAccessStream &results) noexcept {
  winrt::com_ptr<IWICBitmapDecoder> bitmapDecoder;
  winrt::com_ptr<IWICImagingFactory> imagingFactory;
  winrt::check_hresult(WICCreateImagingFactory_Proxy(WINCODEC_SDK_VERSION, imagingFactory.put()));

  if (!results) {
    m_state = ImageState::Error;
    return;
  }

  winrt::com_ptr<IStream> istream;
  winrt::check_hresult(
      CreateStreamOverRandomAccessStream(results.as<IUnknown>().get(), __uuidof(IStream), istream.put_void()));

  if (imagingFactory->CreateDecoderFromStream(
          istream.get(), nullptr, WICDecodeMetadataCacheOnDemand, bitmapDecoder.put()) < 0) {
    m_state = ImageState::Error;
    return;
  }

  winrt::com_ptr<IWICBitmapFrameDecode> decodedFrame;
  winrt::check_hresult(bitmapDecoder->GetFrame(0, decodedFrame.put()));

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

void CompImageComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldImageProps = *std::static_pointer_cast<const facebook::react::ImageProps>(m_props);
  const auto &newImageProps = *std::static_pointer_cast<const facebook::react::ImageProps>(props);

  ensureVisual();

  updateBorderProps(oldImageProps, newImageProps);

  if (oldImageProps.backgroundColor != newImageProps.backgroundColor) {
    auto color = *newImageProps.backgroundColor;

    if (newImageProps.backgroundColor) {
      auto brush = Compositor().CreateColorBrush((*newImageProps.backgroundColor).m_color);
      m_visual.as<winrt::Windows::UI::Composition::SpriteVisual>().Brush(brush);
    } else {
      m_visual.as<winrt::Windows::UI::Composition::SpriteVisual>().Brush(nullptr);
    }
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

void CompImageComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {}

void CompImageComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  // Set Position & Size Properties

  if ((layoutMetrics.displayType != m_layoutMetrics.displayType)) {
    m_visual.IsVisible(layoutMetrics.displayType != facebook::react::DisplayType::None);
  }

  // m_needsBorderUpdate = true;
  m_layoutMetrics = layoutMetrics;

  updateBorderLayoutMetrics();

  m_visual.Size(
      {layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor,
       layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor});
  m_visual.Offset({
      layoutMetrics.frame.origin.x * layoutMetrics.pointScaleFactor,
      layoutMetrics.frame.origin.y * layoutMetrics.pointScaleFactor,
      0.0f,
  });
}

void CompImageComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {
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

void CompImageComponentView::ensureDrawingSurface() noexcept {
  assert(m_context.UIDispatcher().HasThreadAccess());

  if (!m_drawingSurfaceInterop && m_wicbmp) {
    winrt::Windows::UI::Composition::ICompositionDrawingSurface drawingSurface;

    drawingSurface = CompositionGraphicsDevice().CreateDrawingSurface(
        {static_cast<float>(m_imgWidth), static_cast<float>(m_imgHeight)},
        winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
        winrt::Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied);

    // Cache the interop pointer, since that's what we always use.
    drawingSurface.as(m_drawingSurfaceInterop);

    DrawImage();

    // If the rendering device is lost, the application will recreate and replace it. We then
    // own redrawing our pixels.
    if (!m_renderDeviceReplacedToken) {
      m_renderDeviceReplacedToken = CompositionGraphicsDevice().RenderingDeviceReplaced(
          [this](
              winrt::Windows::UI::Composition::ICompositionGraphicsDevice source,
              winrt::Windows::UI::Composition::IRenderingDeviceReplacedEventArgs args) {
            // Draw the text again
            DrawImage();
            return S_OK;
          });
    }

    winrt::Windows::UI::Composition::ICompositionSurface surface;
    m_drawingSurfaceInterop.as(surface);
    auto surfaceBrush = Compositor().CreateSurfaceBrush(surface);

    const auto &imageProps = *std::static_pointer_cast<const facebook::react::ImageProps>(m_props);
    switch (imageProps.resizeMode) {
      case facebook::react::ImageResizeMode::Stretch:
        surfaceBrush.Stretch(winrt::Windows::UI::Composition::CompositionStretch::Fill);
        break;
      case facebook::react::ImageResizeMode::Cover:
        surfaceBrush.Stretch(winrt::Windows::UI::Composition::CompositionStretch::UniformToFill);
        break;
      case facebook::react::ImageResizeMode::Contain:
        surfaceBrush.Stretch(winrt::Windows::UI::Composition::CompositionStretch::Uniform);
        break;
      case facebook::react::ImageResizeMode::Center:
        surfaceBrush.Stretch(winrt::Windows::UI::Composition::CompositionStretch::None);
        break;
      case facebook::react::ImageResizeMode::Repeat:
        surfaceBrush.Stretch(winrt::Windows::UI::Composition::CompositionStretch::UniformToFill);
        // TODO - Hook up repeat
        break;
      default:
        assert(false);
    }

    m_visual.Brush(surfaceBrush);
  }
}

void CompImageComponentView::DrawImage() noexcept {
  // Begin our update of the surface pixels. If this is our first update, we are required
  // to specify the entire surface, which nullptr is shorthand for (but, as it works out,
  // any time we make an update we touch the entire surface, so we always pass nullptr).
  winrt::com_ptr<ID2D1DeviceContext> d2dDeviceContext;
  POINT offset;

  assert(m_context.UIDispatcher().HasThreadAccess());

  if (CheckForDeviceRemoved(m_drawingSurfaceInterop->BeginDraw(
          nullptr, __uuidof(ID2D1DeviceContext), d2dDeviceContext.put_void(), &offset))) {
    const auto &paragraphProps = *std::static_pointer_cast<const facebook::react::ParagraphProps>(m_props);

    winrt::com_ptr<ID2D1Bitmap1> bitmap;
    winrt::check_hresult(d2dDeviceContext->CreateBitmapFromWicBitmap(m_wicbmp.get(), nullptr, bitmap.put()));

    d2dDeviceContext->Clear(D2D1::ColorF(D2D1::ColorF::Black, 0.0f));

    D2D1_RECT_F rect = D2D1::RectF(
        static_cast<float>(offset.x / m_layoutMetrics.pointScaleFactor),
        static_cast<float>(offset.y / m_layoutMetrics.pointScaleFactor),
        static_cast<float>(
            (offset.x + m_imgWidth) / m_layoutMetrics.pointScaleFactor),
        static_cast<float>(
            (offset.y + m_imgHeight) / m_layoutMetrics.pointScaleFactor));

        d2dDeviceContext->DrawBitmap(bitmap.get(), rect);

    // Our update is done. EndDraw never indicates rendering device removed, so any
    // failure here is unexpected and, therefore, fatal.
    winrt::check_hresult(m_drawingSurfaceInterop->EndDraw());
  }
}

void CompImageComponentView::prepareForRecycle() noexcept {}
facebook::react::SharedProps CompImageComponentView::props() noexcept {
  assert(false);
  return {};
}

facebook::react::Tag CompImageComponentView::hitTest(facebook::react::Point pt, facebook::react::Point &localPt)
    const noexcept {
  facebook::react::Point ptLocal{pt.x - m_layoutMetrics.frame.origin.x, pt.y - m_layoutMetrics.frame.origin.y};

  /*
    facebook::react::Tag tag;
    if (std::any_of(m_children.rbegin(), m_children.rend(), [&tag, &ptLocal, &localPt](auto child) {
          tag = static_cast<const CompBaseComponentView *>(child)->hitTest(ptLocal, localPt);
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

void CompImageComponentView::ensureVisual() noexcept {
  if (!m_visual) {
    m_visual = Compositor().CreateSpriteVisual();
    m_brush = Compositor().CreateSurfaceBrush();
  }
}

const winrt::Windows::UI::Composition::Visual CompImageComponentView::Visual() const noexcept {
  return m_visual;
}

} // namespace Microsoft::ReactNative
