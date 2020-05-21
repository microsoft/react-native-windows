// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "DevSupportManager.h"

#include <DevSettings.h>
#include <InstanceManager.h>
#include <unicode.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Networking.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Web.Http.Filters.h>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/Windows.Web.Http.h>
#include "Executors/WebSocketJSExecutor.h"
#include "Utils.h"

using namespace Concurrency;
using namespace std;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::Web::Http;

using winrt::Windows::Foundation::Uri;
using winrt::Windows::Networking::HostName;

namespace Microsoft::React {

DevSupportManager::DevSupportManager() {}

DevSupportManager::~DevSupportManager() {
  StopPollingLiveReload();
}

void DevSupportManager::StopPollingLiveReload() {
  if (!m_liveReloadCts.get_token().is_canceled()) {
    m_liveReloadCts.cancel();
  }
};

JSECreator DevSupportManager::LoadJavaScriptInProxyMode(const DevSettings &settings) {
  return [settings](shared_ptr<ExecutorDelegate> delegate, shared_ptr<MessageQueueThread> jsQueue) {
    auto websocketJSE = make_unique<WebSocketJSExecutor>(delegate, jsQueue);
    websocketJSE->ConnectAsync(DevServerHelper::get_WebsocketProxyUrl(settings.debugHost), settings.errorCallback)
        .wait();
    return websocketJSE;
  };
}

string DevSupportManager::GetJavaScriptFromServer(
    const string &debugHost,
    const string &jsBundleName,
    const string &platform) {
  auto bundleUrl =
      DevServerHelper::get_BundleUrl(debugHost, jsBundleName, platform /*platform*/, "true" /*dev*/, "false" /*hot*/);
  Uri uri(Common::Unicode::Utf8ToUtf16(bundleUrl));
  Filters::HttpBaseProtocolFilter filter;
  filter.CacheControl().ReadBehavior(Filters::HttpCacheReadBehavior::NoCache);
  HttpClient client(filter);
  client.DefaultRequestHeaders().Host(HostName(uri.Host().c_str()));

  HttpRequestMessage request(HttpMethod::Get(), uri);
  HttpResponseMessage response = client.SendRequestAsync(request).get();

  IBuffer buffer0 = response.Content().ReadAsBufferAsync().get();
  auto reader = DataReader::FromBuffer(buffer0);
  reader.UnicodeEncoding(UnicodeEncoding::Utf8);
  auto length = reader.UnconsumedBufferLength();
  string result;
  if (length > 0 || response.IsSuccessStatusCode()) {
    vector<uint8_t> data(length);
    reader.ReadBytes(data);
    result = string(reinterpret_cast<char *>(data.data()), data.size());
  } else {
    m_exceptionCaught = true;
    ostringstream stream;
    stream << R"({"error:")"s << static_cast<int>(response.StatusCode()) << " downloading " << bundleUrl << R"("})"s;
    result = stream.str();
  }

  //return jsStringStream.str();
  return result;
}

void DevSupportManager::StartPollingLiveReload(const string &debugHost, std::function<void()> onChangeCallback) {
  auto t = create_task([this, debugHost, onChangeCallback = move(onChangeCallback)] {
    cancellation_token token = m_liveReloadCts.get_token();
    while (!token.is_canceled()) {
      try {
        Uri uri(Common::Unicode::Utf8ToUtf16(DevServerHelper::get_OnChangeEndpointUrl(debugHost)));
        HttpClient client;
        client.DefaultRequestHeaders().Host(HostName(uri.Host().c_str()));
        client.DefaultRequestHeaders().Connection().TryParseAdd(L"keep-alive");

        HttpResponseMessage response = client.GetAsync(uri, HttpCompletionOption::ResponseHeadersRead).get();

        if (response.StatusCode() == HttpStatusCode::ResetContent /*ResetContent*/ && !token.is_canceled())
          onChangeCallback();

      } catch (const std::exception & /*e*/) {
        // Just let the live reload stop working when the connection fails,
        // rather than bringing down the app.
      }
    }
  });
}

task<void> DevSupportManager::LaunchDevToolsAsync(const string &debugHost, const cancellation_token &token) {
  auto t = task_from_result();

  return t.then([=]() {
    Uri uri(Common::Unicode::Utf8ToUtf16(DevServerHelper::get_LaunchDevToolsCommandUrl(debugHost)));
    HttpClient client;
    client.DefaultRequestHeaders().Host(HostName(uri.Host().c_str()));
    HttpResponseMessage response = client.GetAsync(uri, HttpCompletionOption::ResponseHeadersRead).get();
  });
}

} // namespace Microsoft::React

namespace facebook::react {

shared_ptr<IDevSupportManager> CreateDevSupportManager() {
  return std::make_shared<Microsoft::React::DevSupportManager>();
}

} // namespace facebook::react
