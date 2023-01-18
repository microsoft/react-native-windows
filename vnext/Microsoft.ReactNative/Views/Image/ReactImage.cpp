// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "ReactImage.h"

#include <UI.Xaml.Media.Imaging.h>

#include <winrt/Windows.Security.Cryptography.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/Windows.Web.Http.h>

#include <Utils/ValueUtils.h>
#include <Views/DynamicAutomationPeer.h>
#include "Unicode.h"
#include "XamlView.h"
#include "cdebug.h"

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;
using namespace Windows::UI;
using namespace xaml;
using namespace xaml::Media;
using namespace xaml::Media::Imaging;
using namespace Windows::Web::Http;
using namespace xaml::Automation::Peers;
} // namespace winrt

using Microsoft::Common::Unicode::Utf8ToUtf16;

namespace Microsoft::ReactNative {

/*static*/ winrt::com_ptr<ReactImage> ReactImage::Create() {
  auto reactImage = winrt::make_self<ReactImage>();
  // Grid inheirts the layout direction from parent and mirrors the background image in RTL mode.
  // Forcing the container to LTR mode to avoid the unexpected mirroring behavior.
  reactImage->FlowDirection(xaml::FlowDirection::LeftToRight);
  return reactImage;
}

winrt::Size ReactImage::ArrangeOverride(winrt::Size finalSize) {
  if (m_useCompositionBrush) {
    if (auto brush{Background().try_as<ReactImageBrush>()}) {
      brush->AvailableSize(finalSize);
    }
  } else if (auto brush{Background().try_as<winrt::ImageBrush>()}) {
    brush.Stretch(ResizeModeToStretch(finalSize));
  }

  if (m_imageSource.sourceFormat == ImageSourceFormat::Svg) {
    if (auto const imageBrush{Background().try_as<winrt::ImageBrush>()}) {
      if (auto const svgImageSource{imageBrush.ImageSource().try_as<winrt::SvgImageSource>()}) {
        if (svgImageSource.RasterizePixelWidth() != GetWidth() ||
            svgImageSource.RasterizePixelHeight() != GetHeight()) {
          SetBackground(false);
        }
      }
    }
  }

  return finalSize;
}

winrt::event_token ReactImage::OnLoadEnd(winrt::EventHandler<bool> const &handler) {
  return m_onLoadEndEvent.add(handler);
}

void ReactImage::OnLoadEnd(winrt::event_token const &token) noexcept {
  m_onLoadEndEvent.remove(token);
}

winrt::AutomationPeer ReactImage::OnCreateAutomationPeer() {
  return winrt::make<winrt::Microsoft::ReactNative::implementation::DynamicAutomationPeer>(*this);
}

void ReactImage::ResizeMode(facebook::react::ImageResizeMode value) {
  if (m_resizeMode != value) {
    m_resizeMode = value;

    bool shouldUseCompositionBrush{
        m_resizeMode == facebook::react::ImageResizeMode::Repeat || m_blurRadius > 0 || m_tintColor.A != 0};
    bool switchBrushes{m_useCompositionBrush != shouldUseCompositionBrush};

    if (switchBrushes) {
      m_useCompositionBrush = shouldUseCompositionBrush;
      SetBackground(false);
    } else if (auto brush{Background().try_as<ReactImageBrush>()}) {
      brush->ResizeMode(value);
    } else if (auto bitmapBrush{Background().as<winrt::ImageBrush>()}) {
      bitmapBrush.Stretch(ResizeModeToStretch());
    }
  }
}

void ReactImage::BlurRadius(float value) {
  if (m_blurRadius != value) {
    m_blurRadius = value;

    bool shouldUseCompositionBrush{
        m_resizeMode == facebook::react::ImageResizeMode::Repeat || m_blurRadius > 0 || m_tintColor.A != 0};
    bool switchBrushes{m_useCompositionBrush != shouldUseCompositionBrush};

    if (switchBrushes) {
      m_useCompositionBrush = shouldUseCompositionBrush;
      SetBackground(false);
    } else if (auto brush{Background().try_as<ReactImageBrush>()}) {
      brush->BlurRadius(value);
    }
  }
}

void ReactImage::TintColor(winrt::Color value) {
  bool sameColor{
      value.A == m_tintColor.A && value.R == m_tintColor.R && value.G == m_tintColor.G && value.B == m_tintColor.B};

  if (!sameColor) {
    m_tintColor = value;
    bool shouldUseCompositionBrush{
        m_resizeMode == facebook::react::ImageResizeMode::Repeat || m_blurRadius > 0 || m_tintColor.A != 0};
    bool switchBrushes{m_useCompositionBrush != shouldUseCompositionBrush};

    if (switchBrushes) {
      m_useCompositionBrush = shouldUseCompositionBrush;
      SetBackground(false);
    } else if (auto brush{Background().try_as<ReactImageBrush>()}) {
      brush->TintColor(value);
    }
  }
}

winrt::Stretch ReactImage::ResizeModeToStretch() {
  return ResizeModeToStretch({static_cast<float>(ActualWidth()), static_cast<float>(ActualHeight())});
}

winrt::Stretch ReactImage::ResizeModeToStretch(winrt::Size size) {
  switch (m_resizeMode) {
    case facebook::react::ImageResizeMode::Cover:
      return winrt::Stretch::UniformToFill;
    case facebook::react::ImageResizeMode::Stretch:
      return winrt::Stretch::Fill;
    case facebook::react::ImageResizeMode::Contain:
      return winrt::Stretch::Uniform;
    default: // ResizeMode::Center || ResizeMode::Repeat
      if (m_imageSource.height < size.Height && m_imageSource.width < size.Width) {
        return winrt::Stretch::None;
      } else {
        return winrt::Stretch::Uniform;
      }
  }
}

void ReactImage::Source(ReactImageSource source) {
  if (source.uri.length() == 0) {
    m_onLoadEndEvent(*this, false);
    return;
  }

  try {
    winrt::Uri uri{UriTryCreate(Utf8ToUtf16(source.uri))};
    winrt::hstring scheme{uri ? uri.SchemeName() : L""};
    winrt::hstring ext{uri ? uri.Extension() : L""};

    if (((scheme == L"http") || (scheme == L"https")) && !source.headers.empty()) {
      source.sourceType = ImageSourceType::Download;
    } else if (scheme == L"data") {
      source.sourceType = ImageSourceType::InlineData;
      if (source.uri.find("image/svg+xml;base64") != std::string::npos) {
        source.sourceFormat = ImageSourceFormat::Svg;
      }
    } else if (ext == L".svg" || ext == L".svgz") {
      source.sourceFormat = ImageSourceFormat::Svg;
    }

    m_imageSource = source;

    SetBackground(true);
  } catch (winrt::hresult_error const &) {
    m_onLoadEndEvent(*this, false);
  }
}

template <typename TImage>
std::wstring GetUriFromImage(const TImage &image) {
  return image.UriSource() ? image.UriSource().ToString().c_str() : L"<no Uri available>";
}
template <>
std::wstring GetUriFromImage(const winrt::Uri &uri) {
  return uri != nullptr ? uri.ToString().c_str() : L"<no Uri available>";
}

template <typename TImage>
void ImageFailed(const TImage &image, const xaml::ExceptionRoutedEventArgs &args) {
#ifdef DEBUG
  cdebug << L"Failed to load image " << GetUriFromImage(image) << L" (" << args.ErrorMessage().c_str() << L")\n";
#endif
}

// TSourceFailedEventArgs can be either LoadedImageSourceLoadCompletedEventArgs or
// SvgImageSourceFailedEventArgs, because they both have Status() properties
// and the type of status are both enums with the same meaning
// See LoadedImageSourceLoadStatus and SvgImageSourceLoadStatus.
template <typename TImage, typename TSourceFailedEventArgs>
void ImageFailed(const TImage &image, const TSourceFailedEventArgs &args) {
  // https://docs.microsoft.com/en-us/uwp/api/windows.ui.xaml.media.loadedimagesourceloadstatus
#ifdef DEBUG
  constexpr std::wstring_view statusNames[] = {L"Success", L"NetworkError", L"InvalidFormat", L"Other"};
  const auto status = (int)args.Status();
  assert(0 <= status && status < ARRAYSIZE(statusNames));
  cdebug << L"Failed to load image " << GetUriFromImage(image) << L" (" << statusNames[status] << L")\n";
#endif
}

winrt::fire_and_forget ReactImage::SetBackground(bool fireLoadEndEvent) {
  const ReactImageSource source{m_imageSource};
  winrt::Uri uri{UriTryCreate(Utf8ToUtf16(source.uri))};

  // Increment the image source ID before any co_await calls
  auto currentImageSourceId = ++m_imageSourceId;

  const bool fromStream{
      source.sourceType == ImageSourceType::Download || source.sourceType == ImageSourceType::InlineData};

  winrt::IRandomAccessStream memoryStream{nullptr};

  // get weak reference before any co_await calls
  auto weak_this{get_weak()};

  try {
    if (fromStream) {
      memoryStream = co_await GetImageMemoryStreamAsync(source);

      // Fire failed load event if we're not loading from URI and the memory stream is null.
      if (!memoryStream) {
        if (auto strong_this{weak_this.get()}) {
          strong_this->m_onLoadEndEvent(*strong_this, false);
        }
        co_return;
      }
    }
  } catch (winrt::hresult_error const &) {
    const auto strong_this{weak_this.get()};
    if (strong_this && fireLoadEndEvent) {
      strong_this->m_onLoadEndEvent(*strong_this, false);
    }
    co_return;
  }

  if (auto strong_this{weak_this.get()}) {
    // If the image source has been updated since this operation started, do not continue
    if (currentImageSourceId != strong_this->m_imageSourceId) {
      co_return;
    }

    if (strong_this->m_useCompositionBrush) {
      const auto compositionBrush{ReactImageBrush::Create()};

      compositionBrush->BlurRadius(strong_this->m_blurRadius);
      compositionBrush->TintColor(strong_this->m_tintColor);

      const auto surface = fromStream ? winrt::LoadedImageSurface::StartLoadFromStream(memoryStream)
          : uri                       ? winrt::LoadedImageSurface::StartLoadFromUri(uri)
                                      : nullptr;

      m_sizeChangedRevoker = strong_this->SizeChanged(
          winrt::auto_revoke, [compositionBrush](const auto &, const winrt::SizeChangedEventArgs &args) {
            compositionBrush->AvailableSize(args.NewSize());
          });

      if (strong_this->m_surfaceLoadedRevoker) {
        strong_this->m_surfaceLoadedRevoker.revoke();
      }

      if (surface) {
        strong_this->m_surfaceLoadedRevoker = surface.LoadCompleted(
            winrt::auto_revoke,
            [weak_this, compositionBrush, surface, fireLoadEndEvent, uri](
                winrt::LoadedImageSurface const & /*sender*/,
                winrt::LoadedImageSourceLoadCompletedEventArgs const &args) {
              if (auto strong_this{weak_this.get()}) {
                bool succeeded{false};
                if (args.Status() == winrt::LoadedImageSourceLoadStatus::Success) {
                  winrt::Size size{surface.DecodedPhysicalSize()};
                  strong_this->m_imageSource.height = size.Height;
                  strong_this->m_imageSource.width = size.Width;

                  // If we are dynamically switching the resizeMode to 'repeat', then
                  // the SizeChanged event has already fired and the ReactImageBrush's
                  // size has not been set. Use ActualWidth/Height in that case.
                  if (compositionBrush->AvailableSize() == winrt::Size{0, 0}) {
                    compositionBrush->AvailableSize(
                        {static_cast<float>(strong_this->ActualWidth()),
                         static_cast<float>(strong_this->ActualHeight())});
                  }

                  compositionBrush->Source(surface);
                  compositionBrush->ResizeMode(strong_this->m_resizeMode);
                  compositionBrush->BlurRadius(strong_this->m_blurRadius);
                  compositionBrush->TintColor(strong_this->m_tintColor);

                  strong_this->Background(compositionBrush.as<winrt::XamlCompositionBrushBase>());
                  succeeded = true;
                } else {
                  ImageFailed(uri, args);
                }

                if (fireLoadEndEvent) {
                  strong_this->m_onLoadEndEvent(*strong_this, succeeded);
                }

                strong_this->m_sizeChangedRevoker.revoke();
              }
            });
      }
    } else {
      winrt::ImageBrush imageBrush{strong_this->Background().try_as<winrt::ImageBrush>()};
      bool createImageBrush{!imageBrush};
      if (createImageBrush) {
        imageBrush = winrt::ImageBrush{};
        imageBrush.Stretch(strong_this->ResizeModeToStretch());
      }

      if (source.sourceFormat == ImageSourceFormat::Svg) {
        winrt::SvgImageSource svgImageSource{};

        strong_this->m_svgImageSourceOpenedRevoker =
            svgImageSource.Opened(winrt::auto_revoke, [weak_this, fireLoadEndEvent](const auto &, const auto &) {
              auto strong_this{weak_this.get()};
              if (strong_this && fireLoadEndEvent) {
                strong_this->m_onLoadEndEvent(*strong_this, true);
              }
            });

        strong_this->m_svgImageSourceOpenFailedRevoker = svgImageSource.OpenFailed(
            winrt::auto_revoke, [weak_this, fireLoadEndEvent, svgImageSource](const auto &, const auto &args) {
              auto strong_this{weak_this.get()};
              if (strong_this && fireLoadEndEvent) {
                strong_this->m_onLoadEndEvent(*strong_this, false);
              }
              ImageFailed(svgImageSource, args);
            });

        svgImageSource.RasterizePixelWidth(GetWidth());
        svgImageSource.RasterizePixelHeight(GetHeight());

        imageBrush.ImageSource(svgImageSource);

        if (fromStream) {
          try {
            co_await svgImageSource.SetSourceAsync(memoryStream);
          } catch (const winrt::hresult_error &) {
            /*
                winrt::hresult_canceled
                If the app changes the image source again via SetSourceAsync, SetSource or UriSource while a
               SetSourceAsync call is already in progress, the pending SetSourceAsync action will throw a
               TaskCanceledException and set the Status to Canceled.

                WINCODEC_ERR_BADIMAGE
                In low memory situations (most likely on lower-memory phones), it is possible for an exception to be
               raised with the message "The image is unrecognized" and an HRESULT of 0x88982F60. While this exception
               ordinarily indicates bad data, if your app is close to its memory limit then the cause of the exception
               is likely to be low memory. In that case, we recommend that you free memory and try again.
            */
          }
        } else {
          svgImageSource.UriSource(uri);
        }
      } else {
        winrt::BitmapImage bitmapImage{imageBrush.ImageSource().try_as<winrt::BitmapImage>()};

        if (!bitmapImage) {
          bitmapImage = winrt::BitmapImage{};

          strong_this->m_bitmapImageOpened = bitmapImage.ImageOpened(
              winrt::auto_revoke, [imageBrush, weak_this, fireLoadEndEvent](const auto &, const auto &) {
                imageBrush.Opacity(1);

                auto strong_this{weak_this.get()};
                if (strong_this && fireLoadEndEvent) {
                  if (auto bitmap{imageBrush.ImageSource().try_as<winrt::BitmapImage>()}) {
                    strong_this->m_imageSource.height = bitmap.PixelHeight();
                    strong_this->m_imageSource.width = bitmap.PixelWidth();
                    imageBrush.Stretch(strong_this->ResizeModeToStretch());
                  }

                  strong_this->m_onLoadEndEvent(*strong_this, true);
                }
              });

          strong_this->m_bitmapImageFailed = bitmapImage.ImageFailed(
              winrt::auto_revoke,
              [imageBrush, weak_this, fireLoadEndEvent, bitmapImage](const auto &, const auto &args) {
                imageBrush.Opacity(1);

                auto strong_this{weak_this.get()};
                if (strong_this && fireLoadEndEvent) {
                  strong_this->m_onLoadEndEvent(*strong_this, false);
                }
                ImageFailed(bitmapImage, args);
              });

          imageBrush.ImageSource(bitmapImage);
        }

        if (fromStream) {
          try {
            co_await bitmapImage.SetSourceAsync(memoryStream);
          } catch (const winrt::hresult_error &) {
            /*
                winrt::hresult_canceled
                If the app changes the image source again via SetSourceAsync, SetSource or UriSource while a
               SetSourceAsync call is already in progress, the pending SetSourceAsync action will throw a
               TaskCanceledException and set the Status to Canceled.

                WINCODEC_ERR_BADIMAGE
                In low memory situations (most likely on lower-memory phones), it is possible for an exception to be
               raised with the message "The image is unrecognized" and an HRESULT of 0x88982F60. While this exception
               ordinarily indicates bad data, if your app is close to its memory limit then the cause of the exception
               is likely to be low memory. In that case, we recommend that you free memory and try again.
            */
          }
        } else {
          bitmapImage.UriSource(uri);

          // It is possible that the same URI will be set twice if an intermediate update occurs that requires
          // asynchronous behavior (e.g., when the ImageSourceType is ::Download or ::InlineData). In this case,
          // do not set the opacity to zero as the ImageOpened event will not fire.
          auto currentUri = bitmapImage.UriSource();
          if (uri && currentUri && uri.AbsoluteUri() != currentUri.AbsoluteUri()) {
            // TODO: When we change the source of a BitmapImage, we're getting a flicker of the old image
            // being resized to the size of the new image. This is a temporary workaround.
            imageBrush.Opacity(0);
          }
        }
      }

      if (createImageBrush) {
        strong_this->Background(imageBrush);
      }
    }
  }
}

double ReactImage::GetWidth() {
  return std::isnan(Width()) ? m_imageSource.width : Width();
}

double ReactImage::GetHeight() {
  return std::isnan(Height()) ? m_imageSource.height : Height();
}
} // namespace Microsoft::ReactNative
