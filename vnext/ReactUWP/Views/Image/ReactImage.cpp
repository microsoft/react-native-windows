// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ReactImage.h"

#include <winrt/Windows.Security.Cryptography.h>
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

#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize("", off)
#endif

namespace react {
namespace uwp {

/*static*/ winrt::com_ptr<ReactImage> ReactImage::Create() {
  return winrt::make_self<ReactImage>();
}

winrt::Size ReactImage::ArrangeOverride(winrt::Size finalSize) {
  if (m_useCompositionBrush) {
    auto brush{Background().as<ReactImageBrush>()};
    brush->AvailableSize(finalSize);
  } else {
    m_availableSize = finalSize;
  }

  return finalSize;
}

winrt::event_token ReactImage::OnLoadEnd(winrt::EventHandler<bool> const &handler) {
  return m_onLoadEndEvent.add(handler);
}

void ReactImage::OnLoadEnd(winrt::event_token const &token) noexcept {
  m_onLoadEndEvent.remove(token);
}

winrt::fire_and_forget ReactImage::ResizeMode(react::uwp::ResizeMode value) {
  if (m_resizeMode != value) {
    m_resizeMode = value;

    bool switchBrushes{false};

    if (m_useCompositionBrush != ShouldUseCompositionBrush()) {
      switchBrushes = true;
      m_useCompositionBrush = ShouldUseCompositionBrush();
    }

    if (switchBrushes) {
      co_await Source(m_imageSource, m_memoryStream != nullptr);
    } else {
      if (m_useCompositionBrush) {
        Background().as<ReactImageBrush>()->ResizeMode(m_resizeMode);
      } else {
        const auto bitmapBrush{Background().as<winrt::ImageBrush>()};
        bitmapBrush.Stretch(ResizeModeToStretch(m_resizeMode));
      }
    }
  }
}

bool ReactImage::ShouldUseCompositionBrush() {
  return m_resizeMode == ResizeMode::Repeat;
}

winrt::Stretch ReactImage::ResizeModeToStretch(react::uwp::ResizeMode value) {
  switch (value) {
    case ResizeMode::Cover:
      return winrt::Stretch::UniformToFill;
    case ResizeMode::Stretch:
      return winrt::Stretch::Fill;
    case ResizeMode::Contain:
      return winrt::Stretch::Uniform;
    default:
      const auto bitmap{Background().as<winrt::ImageBrush>().ImageSource().as<winrt::BitmapImage>()};
      if (bitmap.PixelHeight() < m_availableSize.Height && bitmap.PixelWidth() < m_availableSize.Width) {
        return winrt::Stretch::None;
      } else {
        return winrt::Stretch::Uniform;
      }
  }
}

winrt::Windows::Foundation::IAsyncAction ReactImage::Source(ImageSource source, bool useMemoryStreamCache) {
  std::string uriString{source.uri};
  if (uriString.length() == 0) {
    m_onLoadEndEvent(*this, false);
    return;
  }

  if (source.packagerAsset && uriString.find("file://") == 0) {
    uriString.replace(0, 7, source.bundleRootPath);
  }

  winrt::Uri uri{Utf8ToUtf16(uriString)};
  winrt::hstring scheme{uri.SchemeName()};
  bool needsDownload =
      ((scheme == L"http") || (scheme == L"https")) && (m_useCompositionBrush || !source.headers.empty());
  bool inlineData = scheme == L"data";

  // get weak reference before any co_await calls
  auto weak_this{get_weak()};

  try {
    m_imageSource = source;

    if (!useMemoryStreamCache) {
      if (needsDownload) {
        m_memoryStream = co_await GetImageStreamAsync(source);
      } else if (inlineData) {
        m_memoryStream = co_await GetImageInlineDataAsync(source);
      }
    }

    if (auto strong_this{weak_this.get()}) {
      if ((needsDownload || inlineData) && !strong_this->m_memoryStream) {
        strong_this->m_onLoadEndEvent(*strong_this, false);
      }

      if (!needsDownload || strong_this->m_memoryStream) {
        if (strong_this->m_useCompositionBrush) {
          const auto compositionBrush = ReactImageBrush::Create();
          compositionBrush->AvailableSize(strong_this->m_availableSize);
          compositionBrush->ResizeMode(strong_this->m_resizeMode);

          auto surface = (needsDownload || inlineData)
              ? winrt::LoadedImageSurface::StartLoadFromStream(strong_this->m_memoryStream)
              : winrt::LoadedImageSurface::StartLoadFromUri(uri);

          strong_this->m_surfaceLoadedRevoker = surface.LoadCompleted(
              winrt::auto_revoke,
              [weak_this, compositionBrush, surface](
                  winrt::LoadedImageSurface const & /*sender*/,
                  winrt::LoadedImageSourceLoadCompletedEventArgs const &args) {
                if (auto strong_this{weak_this.get()}) {
                  bool succeeded{false};
                  if (args.Status() == winrt::LoadedImageSourceLoadStatus::Success) {
                    compositionBrush->Source(surface);
                    strong_this->Background(compositionBrush.as<winrt::XamlCompositionBrushBase>());
                    succeeded = true;
                  }
                  strong_this->m_onLoadEndEvent(*strong_this, succeeded);
                }
              });

        } else {
          winrt::ImageBrush bitmapBrush{};

          // ImageOpened and ImageFailed are mutually exclusive. One event of the other will
          // always fire whenever an ImageBrush has the ImageSource value set or reset.
          strong_this->m_imageOpenedRevoker =
              bitmapBrush.ImageOpened(winrt::auto_revoke, [weak_this, bitmapBrush](const auto &, const auto &) {
                if (auto strong_this{weak_this.get()}) {
                  const auto bitmap{bitmapBrush.ImageSource().as<winrt::BitmapImage>()};
                  strong_this->m_imageSource.height = bitmap.PixelHeight();
                  strong_this->m_imageSource.width = bitmap.PixelWidth();

                  bitmapBrush.Stretch(strong_this->ResizeModeToStretch(strong_this->m_resizeMode));

                  strong_this->m_onLoadEndEvent(*strong_this, true);
                }
              });

          strong_this->m_imageFailedRevoker =
              bitmapBrush.ImageFailed(winrt::auto_revoke, [weak_this](const auto &, const auto &) {
                if (auto strong_this{weak_this.get()}) {
                  strong_this->m_onLoadEndEvent(*strong_this, false);
                }
              });

          winrt::BitmapImage bitmap{};

          if (needsDownload || inlineData) {
            co_await bitmap.SetSourceAsync(strong_this->m_memoryStream);
          } else {
            bitmap.UriSource(uri);
          }

          bitmapBrush.ImageSource(bitmap);
          strong_this->Background(bitmapBrush);
        }
      }
    } // namespace uwp
  } catch (winrt::hresult_error const &) {
    if (auto strong_this{weak_this.get()}) {
      strong_this->m_onLoadEndEvent(*strong_this, false);
    }
  }
}

winrt::IAsyncOperation<winrt::InMemoryRandomAccessStream> GetImageStreamAsync(ImageSource source) {
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

    if (response.StatusCode() == winrt::HttpStatusCode::Ok) {
      winrt::IInputStream inputStream{co_await response.Content().ReadAsInputStreamAsync()};
      winrt::InMemoryRandomAccessStream memoryStream;
      co_await winrt::RandomAccessStream::CopyAsync(inputStream, memoryStream);
      memoryStream.Seek(0);

      return memoryStream;
    }
  } catch (winrt::hresult_error const &) {
  }

  return nullptr;
}

winrt::IAsyncOperation<winrt::InMemoryRandomAccessStream> GetImageInlineDataAsync(ImageSource source) {
  size_t start = source.uri.find(',');
  if (start == std::string::npos || start + 1 > source.uri.length())
    return nullptr;

  try {
    co_await winrt::resume_background();

    std::string_view base64String(source.uri.c_str() + start + 1, source.uri.length() - start - 1);
    auto buffer =
        winrt::Windows::Security::Cryptography::CryptographicBuffer::DecodeFromBase64String(Utf8ToUtf16(base64String));

    winrt::InMemoryRandomAccessStream memoryStream;
    co_await memoryStream.WriteAsync(buffer);
    memoryStream.Seek(0);

    return memoryStream;
  } catch (winrt::hresult_error const &) {
    // Base64 decode failed
  }

  return nullptr;
}
} // namespace uwp
} // namespace react
