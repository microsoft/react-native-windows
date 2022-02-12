// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "WinRTHttpResource.h"

#include <Utils/CppWinrtLessExceptions.h>
#include <Utils/WinRTConversions.h>
#include <utilities.h>

// Windows API
#include <winrt/Windows.Security.Cryptography.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Web.Http.Headers.h>

using std::function;
using std::scoped_lock;
using std::shared_ptr;
using std::string;

using winrt::fire_and_forget;
using winrt::hresult_error;
using winrt::to_hstring;
using winrt::to_string;
using winrt::Windows::Foundation::Uri;
using winrt::Windows::Security::Cryptography::CryptographicBuffer;
using winrt::Windows::Storage::StorageFile;
using winrt::Windows::Storage::Streams::DataReader;
using winrt::Windows::Storage::Streams::UnicodeEncoding;
using winrt::Windows::Web::Http::HttpBufferContent;
using winrt::Windows::Web::Http::HttpMethod;
using winrt::Windows::Web::Http::HttpRequestMessage;
using winrt::Windows::Web::Http::HttpStreamContent;
using winrt::Windows::Web::Http::HttpStringContent;
using winrt::Windows::Web::Http::IHttpClient;
using winrt::Windows::Web::Http::IHttpContent;
using winrt::Windows::Web::Http::Headers::HttpMediaTypeHeaderValue;

namespace Microsoft::React {

#pragma region WinRTHttpResource

// TODO: Check for multi-thread issues if there are multiple instances.
/*static*/ int64_t WinRTHttpResource::s_lastRequestId = 0;

WinRTHttpResource::WinRTHttpResource(IHttpClient &&client) noexcept : m_client{std::move(client)} {}

WinRTHttpResource::WinRTHttpResource() noexcept : WinRTHttpResource(winrt::Windows::Web::Http::HttpClient()) {}

#pragma region IHttpResource

void WinRTHttpResource::SendRequest(
    string &&method,
    string &&url,
    Headers &&headers,
    BodyData &&bodyData,
    string &&responseType,
    bool useIncrementalUpdates,
    int64_t timeout,
    bool withCredentials,
    std::function<void(int64_t)> &&callback) noexcept /*override*/ {
  auto requestId = ++s_lastRequestId;

  // Enforce supported args
  assert(responseType == "text" || responseType == "base64");

  if (callback) {
    callback(requestId);
  }

  try {
    HttpMethod httpMethod{to_hstring(std::move(method))};
    Uri uri{to_hstring(std::move(url))};
    HttpRequestMessage request{httpMethod, uri};
    HttpMediaTypeHeaderValue contentType{nullptr};
    string contentEncoding;
    string contentLength;

    // Headers are generally case-insensitive
    // https://www.ietf.org/rfc/rfc2616.txt section 4.2
    for (auto &header : headers) {
      if (_stricmp(header.first.c_str(), "content-type") == 0) {
        bool success = HttpMediaTypeHeaderValue::TryParse(to_hstring(header.second), contentType);
        if (!success && m_onError) {
          return m_onError(requestId, "Failed to parse Content-Type");
        }
      } else if (_stricmp(header.first.c_str(), "content-encoding") == 0) {
        contentEncoding = header.second;
      } else if (_stricmp(header.first.c_str(), "content-length") == 0) {
        contentLength = header.second;
      } else if (_stricmp(header.first.c_str(), "authorization") == 0) {
        bool success =
            request.Headers().TryAppendWithoutValidation(to_hstring(header.first), to_hstring(header.second));
        if (!success && m_onError) {
          return m_onError(requestId, "Failed to append Authorization");
        }
      } else {
        request.Headers().Append(to_hstring(header.first), to_hstring(header.second));
      }
    }

    IHttpContent content{nullptr};
    if (BodyData::Type::String == bodyData.Type) {
      content = HttpStringContent{to_hstring(bodyData.Data)};
    } else if (BodyData::Type::Base64 == bodyData.Type) {
      auto buffer = CryptographicBuffer::DecodeFromBase64String(to_hstring(bodyData.Data));
      content = HttpBufferContent{buffer};
    } else if (BodyData::Type::Uri == bodyData.Type) {
      auto file = StorageFile::GetFileFromApplicationUriAsync(Uri{to_hstring(bodyData.Data)}).get();
      auto stream = file.OpenReadAsync().get();
      content = HttpStreamContent{stream};
    } else if (BodyData::Type::Form == bodyData.Type) {
      // TODO: Add support
    } else {
      // BodyData::Type::Empty
      // TODO: Error 'cause unsupported??
    }

    if (content != nullptr) {
      // Attach content headers
      if (contentType) {
        content.Headers().ContentType(contentType);
      }
      if (!contentEncoding.empty()) {
        if (!content.Headers().ContentEncoding().TryParseAdd(to_hstring(contentEncoding))) {
          if (m_onError) {
            m_onError(requestId, "Failed to parse Content-Encoding");
          }
          return;
        }
      }
      if (!contentLength.empty()) {
        const auto contentLengthHeader = _atoi64(contentLength.c_str()); // TODO: Alternatives to _atoi64?
        content.Headers().ContentLength(contentLengthHeader);
      }

      request.Content(content);
    }

    PerformSendRequest(requestId, std::move(request), responseType == "text");
  } catch (std::exception const &e) {
    if (m_onError) {
      m_onError(requestId, e.what());
    }
  } catch (hresult_error const &e) {
    if (m_onError) {
      m_onError(requestId, Utilities::HResultToString(e));
    }
  } catch (...) {
    m_onError(requestId, "Unidentified error sending HTTP request");
  }
}

void WinRTHttpResource::AbortRequest(int64_t requestId) noexcept /*override*/ {
  ResponseType request{nullptr};

  {
    scoped_lock lock{m_mutex};
    auto iter = m_responses.find(requestId);
    if (iter == std::end(m_responses)) {
      return;
    }
    request = iter->second;
  }

  try {
    request.Cancel();
  } catch (hresult_error const &e) {
    m_onError(requestId, Utilities::HResultToString(e));
  }
}

void WinRTHttpResource::ClearCookies() noexcept /*override*/ {
  assert(false);
  // NOT IMPLEMENTED
}

void WinRTHttpResource::SetOnRequest(function<void(int64_t requestId)> &&handler) noexcept /*override*/ {
  m_onRequest = std::move(handler);
}

void WinRTHttpResource::SetOnResponse(function<void(int64_t requestId, Response &&response)> &&handler) noexcept
/*override*/ {
  m_onResponse = std::move(handler);
}

void WinRTHttpResource::SetOnData(function<void(int64_t requestId, std::string &&responseData)> &&handler) noexcept
/*override*/ {
  m_onData = std::move(handler);
}

void WinRTHttpResource::SetOnError(function<void(int64_t requestId, string &&errorMessage)> &&handler) noexcept
/*override*/ {
  m_onError = std::move(handler);
}

#pragma endregion IHttpResource

void WinRTHttpResource::TrackResponse(int64_t requestId, ResponseType response) noexcept {
  scoped_lock lock{m_mutex};
  m_responses[requestId] = response;
}

void WinRTHttpResource::UntrackResponse(int64_t requestId) noexcept {
  scoped_lock lock{m_mutex};
  m_responses.erase(requestId);
}

fire_and_forget
WinRTHttpResource::PerformSendRequest(int64_t requestId, HttpRequestMessage &&request, bool textResponse) noexcept {
  // Keep references after coroutine suspension.
  auto self = shared_from_this();
  auto coRequest = std::move(request);

  // Ensure background thread
  co_await winrt::resume_background();

  try {
    auto sendRequestOp = self->m_client.SendRequestAsync(coRequest);

    self->TrackResponse(requestId, sendRequestOp);

    co_await lessthrow_await_adapter<ResponseType>{sendRequestOp};
    auto result = sendRequestOp.ErrorCode();
    if (result < 0) {
      if (self->m_onError) {
        self->m_onError(requestId, Utilities::HResultToString(std::move(result)));
      }
      self->UntrackResponse(requestId);
      co_return;
    }

    auto response = sendRequestOp.GetResults();
    if (response) {
      if (self->m_onResponse) {
        Headers headers;

        // Gather headers for both the response content and the response itself
        // See Invoke-WebRequest PowerShell cmdlet or Chromium response handling
        for (auto header : response.Headers()) {
          headers.emplace(to_string(header.Key()), to_string(header.Value()));
        }
        for (auto header : response.Content().Headers()) {
          headers.emplace(to_string(header.Key()), to_string(header.Value()));
        }
        string url = to_string(response.RequestMessage().RequestUri().AbsoluteUri());
        self->m_onResponse(
            requestId, {static_cast<int32_t>(response.StatusCode()), std::move(headers), std::move(url)});
      }
    }

    // TODO: Incremental updates?
    if (response && response.Content()) {
      auto inputStream = co_await response.Content().ReadAsInputStreamAsync();
      auto reader = DataReader{inputStream};

      if (textResponse) {
        reader.UnicodeEncoding(UnicodeEncoding::Utf8);
      }

      // Only support response sizes up to 10MB.
      // TODO: WHY????
      co_await reader.LoadAsync(10 * 1024 * 1024);
      auto length = reader.UnconsumedBufferLength();

      if (textResponse) {
        std::vector<uint8_t> data(length);
        reader.ReadBytes(data);
        string responseData = string(Common::Utilities::CheckedReinterpretCast<char *>(data.data()), data.size());

        if (self->m_onData) {
          self->m_onData(requestId, std::move(responseData));
        }
      } else {
        auto buffer = reader.ReadBuffer(length);
        auto data = CryptographicBuffer::EncodeToBase64String(buffer);
        auto responseData = to_string(std::wstring_view(data));

        if (self->m_onData) {
          self->m_onData(requestId, std::move(responseData));
        }
      }
    } else {
      if (self->m_onError) {
        self->m_onError(requestId, response == nullptr ? "request failed" : "No response content");
      }
    }
  } catch (std::exception const &e) {
    if (self->m_onError) {
      self->m_onError(requestId, e.what());
    }
  } catch (hresult_error const &e) {
    if (self->m_onError) {
      self->m_onError(requestId, Utilities::HResultToString(e));
    }
  } catch (...) {
    if (self->m_onError) {
      self->m_onError(requestId, "Unhandled exception during request");
    }
  }

  self->UntrackResponse(requestId);

  // TODO: keep? See https://devblogs.microsoft.com/oldnewthing/?p=106160
  co_return;
}

#pragma endregion WinRTHttpResource

#pragma region IHttpResource

/*static*/ shared_ptr<IHttpResource> IHttpResource::Make() noexcept {
  return std::make_shared<WinRTHttpResource>();
}

#pragma endregion IHttpResource

} // namespace Microsoft::React
