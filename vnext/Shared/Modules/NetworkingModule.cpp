// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <Utils/CppWinrtLessExceptions.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Security.Cryptography.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/Windows.Web.Http.h>
#include "NetworkingModule.h"

#include <future>
#include "Unicode.h"
#include "Utilities.h"

#include <cxxreact/Instance.h>
#include <cxxreact/JsArgumentHelpers.h>

#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize("", off)
#endif

namespace Microsoft::React {

//
// NetworkingModule::NetworkingHelper
//
class NetworkingModule::NetworkingHelper : public std::enable_shared_from_this<NetworkingHelper> {
 public:
  NetworkingHelper(NetworkingModule *parent) : m_parent(parent) {}

  void Disconnect() {
    m_parent = nullptr;
  }

  void SendRequest(
      const std::string &method,
      const std::string &url,
      const folly::dynamic &headers,
      folly::dynamic bodyData,
      const std::string &responseType,
      bool useIncrementalUpdates,
      int64_t timeout,
      Callback cb) noexcept;
  void AbortRequest(int64_t requestId) noexcept;
  void ClearCookies() noexcept;

  // Event helpers
  void sendEvent(std::string &&eventName, folly::dynamic &&parameters);
  void OnResponseReceived(int64_t requestId, winrt::Windows::Web::Http::HttpResponseMessage response);
  void OnDataReceived(int64_t requestId, std::string &&response);
  void OnRequestSuccess(int64_t requestId);
  void OnRequestError(int64_t requestId, std::string &&error, bool isTimeout);

  std::shared_ptr<NetworkingHelper> getSelf() {
    return shared_from_this();
  }

  void AddRequest(
      int64_t requestId,
      winrt::Windows::Foundation::IAsyncOperationWithProgress<
          winrt::Windows::Web::Http::HttpResponseMessage,
          winrt::Windows::Web::Http::HttpProgress> completion) {
    std::scoped_lock lock(m_mutex);
    m_requests[requestId] = completion;
  }
  void RemoveRequest(int64_t requestId) {
    std::scoped_lock lock(m_mutex);
    m_requests.erase(requestId);
  }

 private:
  static void FillHeadersMap(
      folly::dynamic &headersMap,
      winrt::Windows::Foundation::Collections::IMap<winrt::hstring, winrt::hstring> headers);

 private:
  NetworkingModule *m_parent;

  winrt::Windows::Web::Http::HttpClient m_httpClient;
  std::unordered_map<
      int64_t,
      winrt::Windows::Foundation::IAsyncOperationWithProgress<
          winrt::Windows::Web::Http::HttpResponseMessage,
          winrt::Windows::Web::Http::HttpProgress>>
      m_requests;
  std::mutex m_mutex;
  static std::int64_t s_lastRequestId;
};

std::int64_t NetworkingModule::NetworkingHelper::s_lastRequestId = 0;

winrt::fire_and_forget SendRequestAsync(
    std::shared_ptr<NetworkingModule::NetworkingHelper> networking,
    winrt::Windows::Web::Http::HttpClient httpClient,
    winrt::Windows::Web::Http::HttpRequestMessage request,
    bool textResponse,
    int64_t requestId) {
  // NotYetImplemented: set timeout

  try {
    auto completion = httpClient.SendRequestAsync(request);
    networking->AddRequest(requestId, completion);

    // lessthrow
#ifdef DEFAULT_CPPWINRT_EXCEPTIONS
    winrt::Windows::Web::Http::HttpResponseMessage response = co_await completion;
#else
    co_await lessthrow_await_adapter<winrt::Windows::Foundation::IAsyncOperationWithProgress<
        winrt::Windows::Web::Http::HttpResponseMessage,
        winrt::Windows::Web::Http::HttpProgress>>{completion};
    HRESULT hr = completion.ErrorCode();
    if (FAILED(hr)) {
      networking->OnRequestError(requestId, "Unhandled exception during request", false /*isTimeout*/);
      networking->RemoveRequest(requestId);
      co_return;
    }
    winrt::Windows::Web::Http::HttpResponseMessage response = completion.GetResults();
#endif
    if (response != nullptr)
      networking->OnResponseReceived(requestId, response);

    // NotYetImplemented: useIncrementalUpdates

    if (response != nullptr && response.Content() != nullptr) {
      winrt::Windows::Storage::Streams::IInputStream inputStream = co_await response.Content().ReadAsInputStreamAsync();
      auto reader = winrt::Windows::Storage::Streams::DataReader(inputStream);

      if (textResponse)
        reader.UnicodeEncoding(winrt::Windows::Storage::Streams::UnicodeEncoding::Utf8);

      // Only support up to 10MB response sizes
      co_await reader.LoadAsync(10000000);
      uint32_t len = reader.UnconsumedBufferLength();

      if (textResponse) {
        std::vector<uint8_t> data(len);
        reader.ReadBytes(data);
        std::string responseData =
            std::string(Microsoft::Common::Utilities::CheckedReinterpretCast<char *>(data.data()), data.size());

        networking->OnDataReceived(requestId, std::move(responseData));
      } else {
        auto buffer = reader.ReadBuffer(len);
        winrt::hstring data = winrt::Windows::Security::Cryptography::CryptographicBuffer::EncodeToBase64String(buffer);
        std::string responseData = Microsoft::Common::Unicode::Utf16ToUtf8(std::wstring_view(data));

        networking->OnDataReceived(requestId, std::move(responseData));
      }

      networking->OnRequestSuccess(requestId);
    } else {
      networking->OnRequestError(
          requestId, response == nullptr ? "request failed" : "No response content", false /*isTimeout*/);
    }
  } catch (...) {
    networking->OnRequestError(requestId, "Unhandled exception during request", false /*isTimeout*/);
  }

  networking->RemoveRequest(requestId);
  co_return;
}

void NetworkingModule::NetworkingHelper::sendEvent(std::string &&eventName, folly::dynamic &&parameters) {
  if (!m_parent)
    return;

  auto instance = m_parent->getInstance().lock();
  if (instance)
    instance->callJSFunction("RCTDeviceEventEmitter", "emit", folly::dynamic::array(eventName, std::move(parameters)));
}

void NetworkingModule::NetworkingHelper::FillHeadersMap(
    folly::dynamic &headersMap,
    winrt::Windows::Foundation::Collections::IMap<winrt::hstring, winrt::hstring> headers) {
  for (const auto &header : headers) {
    auto name = Microsoft::Common::Unicode::Utf16ToUtf8(header.Key());
    auto value = Microsoft::Common::Unicode::Utf16ToUtf8(header.Value());
    if (headersMap.count(name) == 0) {
      headersMap[name] = value;
    } else {
      headersMap[name] = headersMap[name] + ", " + value;
    }
  }
}

void NetworkingModule::NetworkingHelper::OnResponseReceived(
    int64_t requestId,
    winrt::Windows::Web::Http::HttpResponseMessage response) {
  folly::dynamic headers = folly::dynamic::object();
  FillHeadersMap(headers, response.Headers());
  if (response.Content() != nullptr)
    FillHeadersMap(headers, response.Content().Headers());

  folly::dynamic receiveArgs = folly::dynamic::array(
      requestId,
      static_cast<int64_t>(response.StatusCode()),
      headers,
      Microsoft::Common::Unicode::Utf16ToUtf8(response.RequestMessage().RequestUri().AbsoluteUri()));

  sendEvent("didReceiveNetworkResponse", std::move(receiveArgs));
}

void NetworkingModule::NetworkingHelper::OnDataReceived(int64_t requestId, std::string &&response) {
  folly::dynamic receiveArgs = folly::dynamic::array(requestId, response);

  sendEvent("didReceiveNetworkData", std::move(receiveArgs));
}

void NetworkingModule::NetworkingHelper::OnRequestSuccess(int64_t requestId) {
  folly::dynamic completeArgs = folly::dynamic::array(requestId);

  sendEvent("didCompleteNetworkResponse", std::move(completeArgs));
}

void NetworkingModule::NetworkingHelper::OnRequestError(int64_t requestId, std::string &&error, bool isTimeout) {
  folly::dynamic errorArgs = folly::dynamic::array(requestId, std::move(error));
  if (isTimeout)
    errorArgs.push_back(true);

  sendEvent("didCompleteNetworkResponse", std::move(errorArgs));
}

void AttachContentHeaders(
    winrt::Windows::Web::Http::IHttpContent content,
    winrt::Windows::Web::Http::Headers::HttpMediaTypeHeaderValue contentType,
    std::string contentEncoding,
    std::string contentLength) {
  if (contentType != nullptr)
    content.Headers().ContentType(contentType);
  if (!contentEncoding.empty())
    content.Headers().ContentEncoding().ParseAdd(Microsoft::Common::Unicode::Utf8ToUtf16(contentEncoding));
  if (!contentLength.empty()) {
    const auto contentLengthHeader = _atoi64(contentLength.c_str());
    content.Headers().ContentLength(contentLengthHeader);
  }
}

void AttachMultipartHeaders(winrt::Windows::Web::Http::IHttpContent content, const folly::dynamic &headers) {
  for (auto &header : headers.items()) {
    auto &name = header.first.getString();
    auto &value = header.second.getString();

    content.Headers().Append(
        Microsoft::Common::Unicode::Utf8ToUtf16(name), Microsoft::Common::Unicode::Utf8ToUtf16(value));
  }
}

void NetworkingModule::NetworkingHelper::SendRequest(
    const std::string &method,
    const std::string &url,
    const folly::dynamic &headers,
    folly::dynamic bodyData,
    const std::string &responseType,
    [[maybe_unused]] bool useIncrementalUpdates,
    int64_t /*timeout*/,
    Callback cb) noexcept {
  int64_t requestId = ++s_lastRequestId;

  // Enforce supported args
  assert(responseType == "text" || responseType == "base64" || responseType == "blob");

  // Callback with the requestId
  cb({requestId});

  try {
    winrt::Windows::Web::Http::HttpMethod httpMethod(Microsoft::Common::Unicode::Utf8ToUtf16(method));
    winrt::Windows::Foundation::Uri uri(Microsoft::Common::Unicode::Utf8ToUtf16(url));

    winrt::Windows::Web::Http::HttpRequestMessage request(httpMethod, uri);

    winrt::Windows::Web::Http::Headers::HttpMediaTypeHeaderValue contentType(nullptr);
    std::string contentEncoding;
    std::string contentLength;

    if (!headers.empty()) {
      for (auto &header : headers.items()) {
        auto &name = header.first.getString();
        auto &value = header.second.getString();

        if (_stricmp(name.c_str(), "content-type") == 0)
          winrt::Windows::Web::Http::Headers::HttpMediaTypeHeaderValue::TryParse(
              Microsoft::Common::Unicode::Utf8ToUtf16(value), contentType);
        else if (_stricmp(name.c_str(), "content-encoding") == 0)
          contentEncoding = value;
        else if (_stricmp(name.c_str(), "content-length") == 0)
          contentLength = value;
        else if (_stricmp(name.c_str(), "authorization") == 0 || _stricmp(name.c_str(), "user-agent") == 0)
          request.Headers().TryAppendWithoutValidation(
              Microsoft::Common::Unicode::Utf8ToUtf16(name), Microsoft::Common::Unicode::Utf8ToUtf16(value));
        else
          request.Headers().Append(
              Microsoft::Common::Unicode::Utf8ToUtf16(name), Microsoft::Common::Unicode::Utf8ToUtf16(value));
      }
    }

    if (!bodyData.empty()) {
      winrt::Windows::Web::Http::IHttpContent content(nullptr);
      if (!bodyData["string"].empty()) {
        winrt::Windows::Web::Http::HttpStringContent contentString(
            Microsoft::Common::Unicode::Utf8ToUtf16(bodyData["string"].asString()));
        content = contentString;
      } else if (!bodyData["base64"].empty()) {
        // base64 encoding binary request
        auto buffer = winrt::Windows::Security::Cryptography::CryptographicBuffer::DecodeFromBase64String(
            Microsoft::Common::Unicode::Utf8ToUtf16(bodyData["base64"].asString()));
        winrt::Windows::Web::Http::HttpBufferContent contentBase64(buffer);
        content = contentBase64;
      } else if (!bodyData["uri"].empty()) {
        // file content request
        winrt::Windows::Foundation::Uri uri2(Microsoft::Common::Unicode::Utf8ToUtf16(bodyData["uri"].asString()));
        winrt::Windows::Storage::StorageFile file =
            winrt::Windows::Storage::StorageFile::GetFileFromApplicationUriAsync(uri2).get();
        auto stream = file.OpenReadAsync().get();
        winrt::Windows::Web::Http::HttpStreamContent contentStream(stream);
        content = contentStream;
      } else if (!bodyData["formData"].empty()) {
        auto formData = bodyData["formData"];
        winrt::Windows::Web::Http::HttpMultipartFormDataContent multiPartContent;

        for (auto &formDataPart : formData) {
          if (!formDataPart["string"].empty()) {
            winrt::Windows::Web::Http::HttpStringContent multipartStringValue{
                Microsoft::Common::Unicode::Utf8ToUtf16(formDataPart["string"].asString())};
            AttachMultipartHeaders(multipartStringValue, formDataPart["headers"]);
            multiPartContent.Add(
                multipartStringValue, Microsoft::Common::Unicode::Utf8ToUtf16(formDataPart["fieldName"].asString()));
          } else if (!formDataPart["uri"].empty()) {
            auto filePath = winrt::to_hstring(formDataPart["uri"].asString());
            winrt::Windows::Storage::StorageFile file =
                winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(filePath).get();

            auto stream = file.OpenReadAsync().get();
            winrt::Windows::Web::Http::HttpStreamContent multipartFileValue(stream);
            AttachMultipartHeaders(multipartFileValue, formDataPart["headers"]);
            multiPartContent.Add(
                multipartFileValue, Microsoft::Common::Unicode::Utf8ToUtf16(formDataPart["fieldName"].asString()));
          }
        }

        content = multiPartContent;
      } else {
        // empty request
      }

      if (content != nullptr) {
        AttachContentHeaders(content, contentType, contentEncoding, contentLength);
        request.Content(content);
      }
    }

    SendRequestAsync(getSelf(), m_httpClient, request, responseType == "text", requestId);
  } catch (...) {
    OnRequestError(requestId, "Unhandled exception during request", false /*isTimeout*/);
  }
}

void NetworkingModule::NetworkingHelper::AbortRequest(int64_t requestId) noexcept {
  winrt::Windows::Foundation::IAsyncOperationWithProgress<
      winrt::Windows::Web::Http::HttpResponseMessage,
      winrt::Windows::Web::Http::HttpProgress>
      httpRequest(nullptr);
  {
    std::scoped_lock lock(m_mutex);
    auto iter = m_requests.find(requestId);
    if (iter == end(m_requests))
      return;
    httpRequest = iter->second;
  }

  try {
    httpRequest.Cancel();
  } catch (...) {
    // Error canceling request
  }
}

void NetworkingModule::NetworkingHelper::ClearCookies() noexcept {
  assert(false); // Not implemented
}

//
// NetworkingModule
//

NetworkingModule::NetworkingModule() : m_networking(std::make_shared<NetworkingHelper>(this)) {}

NetworkingModule::~NetworkingModule() {
  m_networking->Disconnect();
}

std::string NetworkingModule::getName() {
  return Name;
}

std::map<std::string, folly::dynamic> NetworkingModule::getConstants() {
  std::map<std::string, folly::dynamic> constants{{}};

  return constants;
}

auto NetworkingModule::getMethods() -> std::vector<Method> {
  std::shared_ptr<NetworkingHelper> networking(m_networking);
  return {
      Method(
          "sendRequest",
          [networking](folly::dynamic args, Callback cb) noexcept {
            auto params = facebook::xplat::jsArgAsObject(args, 0);

            networking->SendRequest(
                params["method"].asString(),
                params["url"].asString(),
                params["headers"],
                params["data"],
                params["responseType"].asString(),
                params["incrementalUpdates"].asBool(),
                static_cast<int64_t>(params["timeout"].asDouble()),
                cb);
          }),
      Method(
          "abortRequest",
          [networking](folly::dynamic args) noexcept {
            networking->AbortRequest(facebook::xplat::jsArgAsInt(args, 0));
          }),
      Method("clearCookies", [networking](folly::dynamic args) noexcept { networking->ClearCookies(); }),
  };
}

} // namespace Microsoft::React
