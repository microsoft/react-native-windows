// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ReactImage.h"

#include <winrt/Windows.Web.Http.h>

#include "unicode.h"

namespace winrt {
  using namespace Windows::Foundation;
  using namespace Windows::Storage::Streams;
  using namespace Windows::UI;
  using namespace Windows::UI::Xaml::Media;
  using namespace Windows::Web::Http;
}

namespace facebook {
  using namespace facebook::react::unicode;
}

#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize( "", off )
#endif

namespace react {
  namespace uwp {

    ReactImage::ReactImage()
    {
      m_brush = ReactImageBrush::Create();
      this->Background(m_brush.as<winrt::XamlCompositionBrushBase>());
    }

    /*static*/ winrt::com_ptr<ReactImage> ReactImage::Create()
    {
      return winrt::make_self<ReactImage>();
    }

    winrt::Size ReactImage::ArrangeOverride(winrt::Size finalSize)
    {
      auto brush{ Background().as<ReactImageBrush>() };
      brush->AvailableSize(finalSize);

      return finalSize;
    }

    winrt::event_token ReactImage::OnLoadEnd(winrt::EventHandler<bool> const& handler)
    {
      return m_onLoadEndEvent.add(handler);
    }

    void ReactImage::OnLoadEnd(winrt::event_token const& token) noexcept
    {
      m_onLoadEndEvent.remove(token);
    }

    winrt::fire_and_forget ReactImage::Source(ImageSource source)
    {
      std::string uriString{ source.uri };
      if (uriString.length() == 0)
      {
        m_onLoadEndEvent(*this, false);
        return;
      }

      if (source.packagerAsset && uriString.find("file://") == 0)
      {
        uriString.replace(0, 7, "ms-appx:///Bundle/");
      }

      winrt::Uri uri{ facebook::utf8ToUtf16(uriString) };
      winrt::hstring scheme{ uri.SchemeName() };
      bool needsDownload = (scheme == L"http") || (scheme == L"https");

      try
      {
        m_imageSource = source;

        winrt::InMemoryRandomAccessStream memoryStream;
        if (needsDownload)
        {
          memoryStream = co_await GetImageStreamAsync(source);

          if (!memoryStream)
          {
            m_onLoadEndEvent(*this, false);
          }
        }

        if (!needsDownload || memoryStream)
        {
          auto surface{ needsDownload ?
            winrt::LoadedImageSurface::StartLoadFromStream(memoryStream) :
            winrt::LoadedImageSurface::StartLoadFromUri(uri) };

          surface.LoadCompleted({ this, &ReactImage::LoadedImageSurfaceHandler });

          m_brush->Source(surface);
        }
      }
      catch (winrt::hresult_error const&)
      {
        m_onLoadEndEvent(*this, false);
      }
    }

    void ReactImage::LoadedImageSurfaceHandler(winrt::LoadedImageSurface const& sender, winrt::LoadedImageSourceLoadCompletedEventArgs const& args)
    {
      bool succeeded{ false };
      if (args.Status() == winrt::LoadedImageSourceLoadStatus::Success)
      {
        m_brush->Source(sender.as<winrt::LoadedImageSurface>());
        succeeded = true;
      }

      m_onLoadEndEvent(*this, succeeded);
    }

    winrt::IAsyncOperation<winrt::InMemoryRandomAccessStream> GetImageStreamAsync(ImageSource source)
    {
      try
      {
        co_await winrt::resume_background();

        auto httpMethod{ source.method.empty() ?
          winrt::HttpMethod::Get() :
          winrt::HttpMethod{facebook::utf8ToUtf16(source.method)}
        };

        winrt::Uri uri{ facebook::utf8ToUtf16(source.uri) };
        winrt::HttpRequestMessage request{ httpMethod, uri };

        if (!source.headers.empty())
        {
          for (auto& header : source.headers.items())
          {
            const std::string& name{ header.first.getString() };
            const std::string& value{ header.second.getString() };

            if (_stricmp(name.c_str(), "authorization") == 0)
            {
              request.Headers().TryAppendWithoutValidation(facebook::utf8ToUtf16(name), facebook::utf8ToUtf16(value));
            }
            else
            {
              request.Headers().Append(facebook::utf8ToUtf16(name), facebook::utf8ToUtf16(value));
            }
          }
        }

        winrt::HttpClient httpClient;
        winrt::HttpResponseMessage response{ co_await httpClient.SendRequestAsync(request) };

        if (response.StatusCode() == winrt::HttpStatusCode::Ok)
        {
          winrt::IInputStream inputStream{ co_await response.Content().ReadAsInputStreamAsync() };
          winrt::InMemoryRandomAccessStream memoryStream;
          co_await winrt::RandomAccessStream::CopyAsync(inputStream, memoryStream);
          memoryStream.Seek(0);

          return memoryStream;
        }
      }
      catch (winrt::hresult_error const&)
      {
      }

      return nullptr;
    }
  }
} // namespace react::uwp
