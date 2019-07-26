// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ReactImage.h"

#include <winrt/Windows.Security.Cryptography.h>
#include <winrt/Windows.Web.Http.h>

#include "unicode.h"

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;
using namespace Windows::UI;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::Web::Http;
} // namespace winrt

namespace facebook {
using namespace facebook::react::unicode;
}

#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize("", off)
#endif

namespace react {
namespace uwp {

ReactImage::ReactImage() {
  m_brush = ReactImageBrush::Create();
  this->Background(m_brush.as<winrt::XamlCompositionBrushBase>());
}

/*static*/ winrt::com_ptr<ReactImage> ReactImage::Create() {
  return winrt::make_self<ReactImage>();
}

winrt::Size ReactImage::ArrangeOverride(winrt::Size finalSize) {
  auto brush{Background().as<ReactImageBrush>()};
  brush->AvailableSize(finalSize);

  return finalSize;
}

winrt::event_token ReactImage::OnLoadEnd(
    winrt::EventHandler<bool> const &handler) {
  return m_onLoadEndEvent.add(handler);
}

void ReactImage::OnLoadEnd(winrt::event_token const &token) noexcept {
  m_onLoadEndEvent.remove(token);
}

winrt::fire_and_forget ReactImage::Source(ImageSource source) {
  std::string uriString{source.uri};
  if (uriString.length() == 0) {
    m_onLoadEndEvent(*this, false);
    return;
  }

  if (source.packagerAsset && uriString.find("file://") == 0) {
    uriString.replace(0, 7, source.bundleRootPath);
  }

  winrt::Uri uri{facebook::utf8ToUtf16(uriString)};
  winrt::hstring scheme{uri.SchemeName()};
  bool needsDownload = (scheme == L"http") || (scheme == L"https");
  bool inlineData = scheme == L"data";
  // get weak reference before any co_await calls
  auto weak_this{get_weak()};

  try {
    m_imageSource = source;

    winrt::InMemoryRandomAccessStream memoryStream;
    if (needsDownload) {
      memoryStream = co_await GetImageStreamAsync(source);
    } else if (inlineData) {
      memoryStream = co_await GetImageInlineDataAsync(source);
    }

    if (auto strong_this{weak_this.get()}) {
      if ((needsDownload || inlineData) && !memoryStream) {
        strong_this->m_onLoadEndEvent(*strong_this, false);
      }

      if (!needsDownload || memoryStream) {
        auto surface = needsDownload || inlineData
            ? winrt::LoadedImageSurface::StartLoadFromStream(memoryStream)
            : winrt::LoadedImageSurface::StartLoadFromUri(uri);

        strong_this->m_surfaceLoadedRevoker = surface.LoadCompleted(
            winrt::auto_revoke,
            [weak_this, surface](
                winrt::LoadedImageSurface const & /*sender*/,
                winrt::LoadedImageSourceLoadCompletedEventArgs const &args) {
              if (auto strong_this{weak_this.get()}) {
                bool succeeded{false};
                if (args.Status() ==
                    winrt::LoadedImageSourceLoadStatus::Success) {
                  strong_this->m_brush->Source(surface);
                  succeeded = true;
                }
                strong_this->m_onLoadEndEvent(*strong_this, succeeded);
              }
            });
      }
    }
  } catch (winrt::hresult_error const &) {
    if (auto strong_this{weak_this.get()})
      strong_this->m_onLoadEndEvent(*strong_this, false);
  }
} // namespace uwp

winrt::IAsyncOperation<winrt::InMemoryRandomAccessStream> GetImageStreamAsync(
    ImageSource source) {
  try {
    co_await winrt::resume_background();

    auto httpMethod{
        source.method.empty()
            ? winrt::HttpMethod::Get()
            : winrt::HttpMethod{facebook::utf8ToUtf16(source.method)}};

    winrt::Uri uri{facebook::utf8ToUtf16(source.uri)};
    winrt::HttpRequestMessage request{httpMethod, uri};

    if (!source.headers.empty()) {
      for (auto &header : source.headers.items()) {
        const std::string &name{header.first.getString()};
        const std::string &value{header.second.getString()};

        if (_stricmp(name.c_str(), "authorization") == 0) {
          request.Headers().TryAppendWithoutValidation(
              facebook::utf8ToUtf16(name), facebook::utf8ToUtf16(value));
        } else {
          request.Headers().Append(
              facebook::utf8ToUtf16(name), facebook::utf8ToUtf16(value));
        }
      }
    }

    winrt::HttpClient httpClient;
    winrt::HttpResponseMessage response{
        co_await httpClient.SendRequestAsync(request)};

    if (response.StatusCode() == winrt::HttpStatusCode::Ok) {
      winrt::IInputStream inputStream{
          co_await response.Content().ReadAsInputStreamAsync()};
      winrt::InMemoryRandomAccessStream memoryStream;
      co_await winrt::RandomAccessStream::CopyAsync(inputStream, memoryStream);
      memoryStream.Seek(0);

      return memoryStream;
    }
  } catch (winrt::hresult_error const &) {
  }

  return nullptr;
}

winrt::IAsyncOperation<winrt::InMemoryRandomAccessStream>
GetImageInlineDataAsync(ImageSource source) {
  size_t start = source.uri.find(',');
  if (start == std::string::npos || start + 1 > source.uri.length())
    return nullptr;

  try {
    co_await winrt::resume_background();

    std::string_view base64String(
        source.uri.c_str() + start + 1, source.uri.length() - start - 1);
    auto buffer = winrt::Windows::Security::Cryptography::CryptographicBuffer::
        DecodeFromBase64String(
            facebook::react::unicode::utf8ToUtf16(base64String));

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
