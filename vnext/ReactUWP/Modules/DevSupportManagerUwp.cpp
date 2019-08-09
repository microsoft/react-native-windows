// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "pch.h"

#include "DevSupportManagerUwp.h"

#include <ReactWindowsCore/DevServerHelper.h>
#include <ReactWindowsCore/DevSettings.h>

#include <Executors/WebSocketJSExecutorUwp.h>

#include "unicode.h"
#include "utilities.h"

#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Web.Http.Filters.h>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/Windows.Web.Http.h>

#pragma warning(push)
#pragma warning(disable : 4146 4244 4068 4251 4101 4267 4804 4309)
#include <cxxreact/JSExecutor.h>
#include <cxxreact/MessageQueueThread.h>
#pragma warning(pop)

#include <future>

#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize("", off)
#endif

using namespace facebook::react;

namespace react {
namespace uwp {

std::future<std::pair<std::string, bool>> DownloadFromAsync(
    const std::string &url) {
  winrt::Windows::Web::Http::Filters::HttpBaseProtocolFilter filter;
  filter.CacheControl().ReadBehavior(
      winrt::Windows::Web::Http::Filters::HttpCacheReadBehavior::NoCache);
  winrt::Windows::Web::Http::HttpClient httpClient(filter);
  winrt::Windows::Foundation::Uri uri(
      facebook::react::unicode::utf8ToUtf16(url));

  co_await winrt::resume_background();

  winrt::Windows::Web::Http::HttpRequestMessage request(
      winrt::Windows::Web::Http::HttpMethod::Get(), uri);
  winrt::Windows::Web::Http::HttpResponseMessage response =
      co_await httpClient.SendRequestAsync(request);

  winrt::Windows::Storage::Streams::IBuffer buffer =
      co_await response.Content().ReadAsBufferAsync();
  auto reader =
      winrt::Windows::Storage::Streams::DataReader::FromBuffer(buffer);

  reader.UnicodeEncoding(
      winrt::Windows::Storage::Streams::UnicodeEncoding::Utf8);
  uint32_t len = reader.UnconsumedBufferLength();
  std::string result;
  if (len > 0 || response.IsSuccessStatusCode()) {
    std::vector<uint8_t> data(len);
    reader.ReadBytes(data);
    result = std::string(reinterpret_cast<char *>(data.data()), data.size());
  } else {
    std::ostringstream sstream;
    sstream << "HTTP Error " << static_cast<int>(response.StatusCode())
            << " downloading " << url;
    result = sstream.str();
  }

  co_return std::make_pair(std::move(result), response.IsSuccessStatusCode());
}

void DevSupportManager::LaunchDevTools(
    const facebook::react::DevSettings &settings) {
  DownloadFromAsync(
      facebook::react::DevServerHelper::get_LaunchDevToolsCommandUrl(
          settings.debugHost))
      .get();
}

facebook::react::JSECreator DevSupportManager::LoadJavaScriptInProxyMode(
    const facebook::react::DevSettings &settings) {
  // Reset exception state since client is requesting new service
  m_exceptionCaught = false;

  try {
    LaunchDevTools(settings);

    return [this, settings](
               std::shared_ptr<facebook::react::ExecutorDelegate> delegate,
               std::shared_ptr<facebook::react::MessageQueueThread> jsQueue) {
      auto websocketJSE =
          std::make_unique<WebSocketJSExecutor>(delegate, jsQueue);
      try {
        websocketJSE
            ->ConnectAsync(
                facebook::react::DevServerHelper::get_WebsocketProxyUrl(
                    settings.debugHost),
                settings.errorCallback)
            .get();
      } catch (...) {
        m_exceptionCaught = true;
      }

      return websocketJSE;
    };
  } catch (winrt::hresult_error const &e) {
    m_exceptionCaught = true;
    throw std::exception(facebook::react::unicode::utf16ToUtf8(
                             e.message().c_str(), e.message().size())
                             .c_str());
  }
}

DevSupportManager::~DevSupportManager() {
  StopPollingLiveReload();
}

// helper function to check if failure is due to timeout.
bool is_timeout(const std::wstring &msg) {
  if (msg.find(L"The operation timed out") != std::string::npos /* WinHTTP */ ||
      msg.find(L"The operation was timed out") !=
          std::string::npos /* IXmlHttpRequest2 */) {
    return true;
  }
  return false;
}

bool is_cancelled(const std::wstring &msg) {
  if (msg.find(L"operation canceled") !=
      std::string::npos /* task cancelled */) {
    return true;
  }
  return false;
}

std::future<winrt::Windows::Web::Http::HttpStatusCode> PollForLiveReload(
    const std::string &url) {
  winrt::Windows::Web::Http::HttpClient httpClient;
  winrt::Windows::Foundation::Uri uri(
      facebook::react::unicode::utf8ToUtf16(url));
  httpClient.DefaultRequestHeaders().Connection().TryParseAdd(L"keep-alive");

  winrt::Windows::Web::Http::HttpResponseMessage responseMessage;
  responseMessage = co_await httpClient.GetAsync(
      uri,
      winrt::Windows::Web::Http::HttpCompletionOption::ResponseHeadersRead);

  co_return responseMessage.StatusCode();
}

void DevSupportManager::StartPollingLiveReload(
    const std::string &debugHost,
    std::function<void()> onChangeCallback) {
  m_cancellation_token = false;

  std::string refreshUrl =
      facebook::react::DevServerHelper::get_OnChangeEndpointUrl(debugHost);
  auto task = [refreshUrl, onChangeCallback = move(onChangeCallback), this](
                  const std::atomic_bool &cancelled) {
    while (!cancelled) {
      try {
        auto statusCode = PollForLiveReload(refreshUrl).get();
        if (statusCode ==
                winrt::Windows::Web::Http::HttpStatusCode::ResetContent &&
            !cancelled) {
          onChangeCallback();
        }
      } catch (winrt::hresult_error const &e) {
        // Continue to poll on known error conditions
        HRESULT hr = e.code();
        if (hr == WININET_E_INVALID_SERVER_RESPONSE)
          continue;

        // Just let the live reload stop working when the connection fails,
        // rather than bringing down the app.
        std::string errorMessage = "Live Reload Stopped:" +
            facebook::react::unicode::utf16ToUtf8(
                                       e.message().c_str(), e.message().size());
        OutputDebugStringA(errorMessage.c_str());
        m_exceptionCaught = true;
        break;
      }
    }
  };

  std::thread t(std::move(task), std::ref(m_cancellation_token));
  t.detach();
}

void DevSupportManager::StopPollingLiveReload() {
  m_cancellation_token = true;
}

std::string DevSupportManager::GetJavaScriptFromServer(
    const std::string &debugHost,
    const std::string &jsBundleName,
    const std::string &platform) {
  // Reset exception state since client is requesting new service
  m_exceptionCaught = false;

  auto bundleUrl = facebook::react::DevServerHelper::get_BundleUrl(
      debugHost, jsBundleName, platform, "true" /*dev*/, "false" /*hot*/);
  try {
    std::string s;
    bool success;
    std::tie(s, success) = DownloadFromAsync(bundleUrl).get();

    if (!success)
      m_exceptionCaught = true;

    return s;
  } catch (winrt::hresult_error const &e) {
    m_exceptionCaught = true;
    return "Error:" +
        facebook::react::unicode::utf16ToUtf8(
               e.message().c_str(), e.message().size());
  }
}

} // namespace uwp
} // namespace react

namespace facebook {
namespace react {

std::shared_ptr<IDevSupportManager> CreateDevSupportManager() {
  return std::make_shared<::react::uwp::DevSupportManager>();
}

} // namespace react
} // namespace facebook
