// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ReactImage.h"

#include <winrt/Windows.UI.Xaml.Media.Imaging.h>

#include <winrt/Windows.Security.Cryptography.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/Windows.Web.Http.h>

#include "Unicode.h"

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::Web::Http;
} // namespace winrt

using Microsoft::Common::Unicode::Utf8ToUtf16;

namespace react {
namespace uwp {

/*static*/ winrt::com_ptr<ReactImage> ReactImage::Create() {
  auto reactImage = winrt::make_self<ReactImage>();
  // Grid inheirts the layout direction from parent and mirrors the background image in RTL mode.
  // Forcing the container to LTR mode to avoid the unexpected mirroring behavior.
  reactImage->FlowDirection(winrt::FlowDirection::LeftToRight);
  return reactImage;
}

winrt::Size ReactImage::ArrangeOverride(winrt::Size finalSize) {
  if (m_useCompositionBrush) {
    if (auto brush{Background().try_as<ReactImageBrush>()}) {
      brush->AvailableSize(finalSize);
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

void ReactImage::ResizeMode(react::uwp::ResizeMode value) {
  if (m_resizeMode != value) {
    m_resizeMode = value;

    bool shouldUseCompositionBrush{m_resizeMode == ResizeMode::Repeat};
    bool switchBrushes{m_useCompositionBrush != shouldUseCompositionBrush};

    if (switchBrushes) {
      m_useCompositionBrush = shouldUseCompositionBrush;
      SetBackground(false);
    } else if (auto bitmapBrush{Background().as<winrt::ImageBrush>()}) {
      bitmapBrush.Stretch(ResizeModeToStretch(m_resizeMode));
    }
  }
}

winrt::Stretch ReactImage::ResizeModeToStretch(react::uwp::ResizeMode value) {
  switch (value) {
    case ResizeMode::Cover:
      return winrt::Stretch::UniformToFill;
    case ResizeMode::Stretch:
      return winrt::Stretch::Fill;
    case ResizeMode::Contain:
      return winrt::Stretch::Uniform;
    default: // ResizeMode::Center
      // This function should never be called for the 'repeat' resizeMode case.
      // That is handled by the shouldUseCompositionBrush/switchBrushes code path.
      assert(value != ResizeMode::Repeat);

      if (m_imageSource.height < ActualHeight() && m_imageSource.width < ActualWidth()) {
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
    winrt::Uri uri{Utf8ToUtf16(source.uri)};
    winrt::hstring scheme{uri.SchemeName()};
    winrt::hstring ext{uri.Extension()};

    if (((scheme == L"http") || (scheme == L"https")) && !source.headers.empty()) {
      source.sourceType = ImageSourceType::Download;
    } else if (scheme == L"data") {
      source.sourceType = ImageSourceType::InlineData;
    } else if (ext == L".svg" || ext == L".svgz") {
      source.sourceType = ImageSourceType::Svg;
    }

    m_imageSource = source;

    SetBackground(true);
  } catch (winrt::hresult_error const &) {
    m_onLoadEndEvent(*this, false);
  }
}

winrt::IAsyncOperation<winrt::InMemoryRandomAccessStream> ReactImage::GetImageMemoryStreamAsync(
    ReactImageSource source) {
  switch (source.sourceType) {
    case ImageSourceType::Download:
      co_return co_await GetImageStreamAsync(source);
    case ImageSourceType::InlineData:
      co_return co_await GetImageInlineDataAsync(source);
    default: // ImageSourceType::Uri
      co_return nullptr;
  }
}

winrt::fire_and_forget ReactImage::SetBackground(bool fireLoadEndEvent) {
  const ReactImageSource source{m_imageSource};
  const winrt::Uri uri{Utf8ToUtf16(source.uri)};
  const bool fromStream{source.sourceType == ImageSourceType::Download ||
                        source.sourceType == ImageSourceType::InlineData};

  winrt::InMemoryRandomAccessStream memoryStream{nullptr};

  // get weak reference before any co_await calls
  auto weak_this{get_weak()};

  try {
    memoryStream = co_await GetImageMemoryStreamAsync(source);

    // Fire failed load event if we're not loading from URI and the memory stream is null.
    if (fromStream && !memoryStream) {
      if (auto strong_this{weak_this.get()}) {
        strong_this->m_onLoadEndEvent(*strong_this, false);
      }
      co_return;
    }
  } catch (winrt::hresult_error const &) {
    const auto strong_this{weak_this.get()};
    if (strong_this && fireLoadEndEvent) {
      strong_this->m_onLoadEndEvent(*strong_this, false);
    }
  }

  if (auto strong_this{weak_this.get()}) {
    if (strong_this->m_useCompositionBrush) {
      const auto compositionBrush{ReactImageBrush::Create()};
      compositionBrush->ResizeMode(strong_this->m_resizeMode);

      const auto surface = fromStream ? winrt::LoadedImageSurface::StartLoadFromStream(memoryStream)
                                      : winrt::LoadedImageSurface::StartLoadFromUri(uri);

      m_sizeChangedRevoker = strong_this->SizeChanged(
          winrt::auto_revoke, [compositionBrush](const auto &, const winrt::SizeChangedEventArgs &args) {
            compositionBrush->AvailableSize(args.NewSize());
          });

      strong_this->m_surfaceLoadedRevoker = surface.LoadCompleted(
          winrt::auto_revoke,
          [weak_this, compositionBrush, surface, fireLoadEndEvent](
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
                // size has not been set. Use ActualSize in that case.
                if (compositionBrush->AvailableSize() == winrt::Size{0, 0}) {
                  compositionBrush->AvailableSize(strong_this->ActualSize());
                }

                compositionBrush->Source(surface);
                strong_this->Background(compositionBrush.as<winrt::XamlCompositionBrushBase>());
                succeeded = true;
              }

              if (fireLoadEndEvent) {
                strong_this->m_onLoadEndEvent(*strong_this, succeeded);
              }

              strong_this->m_sizeChangedRevoker.revoke();
            }
          });
    } else {
      winrt::ImageBrush imageBrush{strong_this->Background().try_as<winrt::ImageBrush>()};
      bool createImageBrush{!imageBrush};
      if (createImageBrush) {
        imageBrush = winrt::ImageBrush{};

        // ImageOpened and ImageFailed are mutually exclusive. One event of the other will
        // always fire whenever an ImageBrush has the ImageSource value set or reset.
        strong_this->m_imageBrushOpenedRevoker = imageBrush.ImageOpened(
            winrt::auto_revoke, [weak_this, imageBrush, fireLoadEndEvent](const auto &, const auto &) {
              if (auto strong_this{weak_this.get()}) {
                if (auto bitmap{imageBrush.ImageSource().try_as<winrt::BitmapImage>()}) {
                  strong_this->m_imageSource.height = bitmap.PixelHeight();
                  strong_this->m_imageSource.width = bitmap.PixelWidth();
                }

                imageBrush.Stretch(strong_this->ResizeModeToStretch(strong_this->m_resizeMode));

                if (fireLoadEndEvent) {
                  strong_this->m_onLoadEndEvent(*strong_this, true);
                }
              }
            });

        strong_this->m_imageBrushFailedRevoker =
            imageBrush.ImageFailed(winrt::auto_revoke, [weak_this, fireLoadEndEvent](const auto &, const auto &) {
              const auto strong_this{weak_this.get()};
              if (strong_this && fireLoadEndEvent) {
                strong_this->m_onLoadEndEvent(*strong_this, false);
              }
            });
      }

      if (source.sourceType == ImageSourceType::Svg) {
        winrt::SvgImageSource svgImageSource{imageBrush.ImageSource().try_as<winrt::SvgImageSource>()};

        if (!svgImageSource) {
          svgImageSource = winrt::SvgImageSource{};

          strong_this->m_svgImageSourceOpenedRevoker =
              svgImageSource.Opened(winrt::auto_revoke, [weak_this, fireLoadEndEvent](const auto &, const auto &) {
                auto strong_this{weak_this.get()};
                if (strong_this && fireLoadEndEvent) {
                  strong_this->m_onLoadEndEvent(*strong_this, true);
                }
              });

          strong_this->m_svgImageSourceOpenFailedRevoker =
              svgImageSource.OpenFailed(winrt::auto_revoke, [weak_this, fireLoadEndEvent](const auto &, const auto &) {
                auto strong_this{weak_this.get()};
                if (strong_this && fireLoadEndEvent) {
                  strong_this->m_onLoadEndEvent(*strong_this, false);
                }
              });

          imageBrush.ImageSource(svgImageSource);
        }

        svgImageSource.UriSource(uri);

      } else {
        winrt::BitmapImage bitmapImage{imageBrush.ImageSource().try_as<winrt::BitmapImage>()};

        if (!bitmapImage) {
          bitmapImage = winrt::BitmapImage{};

          strong_this->m_bitmapImageOpened = bitmapImage.ImageOpened(
              winrt::auto_revoke, [imageBrush](const auto &, const auto &) { imageBrush.Opacity(1); });

          imageBrush.ImageSource(bitmapImage);
        }

        if (fromStream) {
          co_await bitmapImage.SetSourceAsync(memoryStream);
        } else {
          bitmapImage.UriSource(uri);

          // TODO: When we change the source of a BitmapImage, we're getting a flicker of the old image
          // being resized to the size of the new image. This is a temporary workaround.
          imageBrush.Opacity(0);
        }
      }

      if (createImageBrush) {
        strong_this->Background(imageBrush);
      }
    }
  }
}

winrt::IAsyncOperation<winrt::InMemoryRandomAccessStream> GetImageStreamAsync(ReactImageSource source) {
  try {
    co_await winrt::resume_background();

    auto httpMethod{source.method.empty() ? winrt::HttpMethod::Get() : winrt::HttpMethod{Utf8ToUtf16(source.method)}};

    winrt::Uri uri{Utf8ToUtf16(source.uri)};
    winrt::HttpRequestMessage request{httpMethod, uri};

    if (!source.headers.empty()) {
      for (auto &header : source.headers.items()) {
        const std::string &name{header.first.getString()};
        const std::string &value{header.second.getString()};

        if (_stricmp(name.c_str(), "authorization") == 0) {
          request.Headers().TryAppendWithoutValidation(Utf8ToUtf16(name), Utf8ToUtf16(value));
        } else {
          request.Headers().Append(Utf8ToUtf16(name), Utf8ToUtf16(value));
        }
      }
    }

    winrt::HttpClient httpClient;
    winrt::HttpResponseMessage response{co_await httpClient.SendRequestAsync(request)};

    if (response && response.StatusCode() == winrt::HttpStatusCode::Ok) {
      winrt::IInputStream inputStream{co_await response.Content().ReadAsInputStreamAsync()};
      winrt::InMemoryRandomAccessStream memoryStream;
      co_await winrt::RandomAccessStream::CopyAsync(inputStream, memoryStream);
      memoryStream.Seek(0);

      co_return memoryStream;
    }
  } catch (winrt::hresult_error const &) {
  }

  co_return nullptr;
}

winrt::IAsyncOperation<winrt::InMemoryRandomAccessStream> GetImageInlineDataAsync(ReactImageSource source) {
  size_t start = source.uri.find(',');
  if (start == std::string::npos || start + 1 > source.uri.length())
    co_return nullptr;

  try {
    co_await winrt::resume_background();

    std::string_view base64String(source.uri.c_str() + start + 1, source.uri.length() - start - 1);
    auto buffer =
        winrt::Windows::Security::Cryptography::CryptographicBuffer::DecodeFromBase64String(Utf8ToUtf16(base64String));

    winrt::InMemoryRandomAccessStream memoryStream;
    co_await memoryStream.WriteAsync(buffer);
    memoryStream.Seek(0);

    co_return memoryStream;
  } catch (winrt::hresult_error const &) {
    // Base64 decode failed
  }

  co_return nullptr;
}
} // namespace uwp
} // namespace react
