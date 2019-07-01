// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "DevSupportManager.h"

#include <DevSettings.h>
#include <InstanceManager.h>

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include "Utils.h"
#include "WebSocketJSExecutor.h"

using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::beast;
using namespace boost::beast::http;
using namespace Concurrency;
using namespace std;

namespace facebook {
namespace react {

DevSupportManager::DevSupportManager() : m_resolver{m_context} {}

DevSupportManager::~DevSupportManager() {
  StopPollingLiveReload();
}

void DevSupportManager::StopPollingLiveReload() {
  if (!m_liveReloadCts.get_token().is_canceled()) {
    m_liveReloadCts.cancel();
  }
};

JSECreator DevSupportManager::LoadJavaScriptInProxyMode(
    const DevSettings &settings) {
  return [settings](
             shared_ptr<ExecutorDelegate> delegate,
             shared_ptr<MessageQueueThread> jsQueue) {
    auto websocketJSE = make_unique<WebSocketJSExecutor>(delegate, jsQueue);
    websocketJSE
        ->ConnectAsync(
            DevServerHelper::get_WebsocketProxyUrl(settings.debugHost),
            settings.errorCallback)
        .wait();
    return websocketJSE;
  };
}

string DevSupportManager::GetJavaScriptFromServer(
    const string &debugHost,
    const string &jsBundleName,
    const string &platform) {
  auto bundleUrl = DevServerHelper::get_BundleUrl(
      debugHost,
      jsBundleName,
      platform /*platform*/,
      "true" /*dev*/,
      "false" /*hot*/);

  try {
    Url url(bundleUrl);
    auto const resolveResult = m_resolver.resolve(url.host, url.port);
    tcp::socket socket{m_context};
    connect(socket, resolveResult);

    request<string_body> request{verb::get, url.Target(), 11};
    request.set(field::host, url.host);
    request.set(field::user_agent, BOOST_BEAST_VERSION_STRING);

    write(socket, request);

    flat_buffer buffer;
    response<string_body> response;

    parser<false, string_body> p{std::move(response)};
    p.body_limit(
        25 * 1024 *
        1024); // 25MB (boost default of 1MB is too small for dev bundles)

    read(socket, buffer, p.base());
    response = p.release();
    std::stringstream jsStringStream;
    jsStringStream << response.body();
    // TODO: Check if UTF-8 processing is required.

    return jsStringStream.str();
  } catch (const std::exception &e) {
    return e.what();
  }
}

void DevSupportManager::StartPollingLiveReload(
    const string &debugHost,
    std::function<void()> onChangeCallback) {
  auto t = create_task([this,
                        debugHost,
                        onChangeCallback = move(onChangeCallback)] {
    cancellation_token token = m_liveReloadCts.get_token();
    while (!token.is_canceled()) {
      try {
        Url url(DevServerHelper::get_OnChangeEndpointUrl(debugHost));
        auto const resolveResult = m_resolver.resolve(url.host, url.port);
        tcp::socket socket{m_context};
        connect(socket, resolveResult);

        request<string_body> request{verb::get, url.Target(), 11 /*HTTP 1.1*/};
        request.set(field::host, url.host);
        request.set(field::user_agent, BOOST_BEAST_VERSION_STRING);
        request.set(field::connection, "keep-alive");

        write(socket, request);
        flat_buffer buffer;
        response<dynamic_body> response;
        read(socket, buffer, response);

        if (response.result_int() == 205 /*ResetContent*/ &&
            !token.is_canceled())
          onChangeCallback();
      } catch (const std::exception & /*e*/) {
        // Just let the live reload stop working when the connection fails,
        // rather than bringing down the app.
      }
    }
  });
}

task<void> DevSupportManager::LaunchDevToolsAsync(
    const string &debugHost,
    const cancellation_token &token) {
  auto t = task_from_result();

  return t.then([=]() {
    Url url(DevServerHelper::get_LaunchDevToolsCommandUrl(debugHost));
    auto const resolveResult = m_resolver.resolve(url.host, url.port);
    tcp::socket socket{m_context};
    connect(socket, resolveResult);

    request<string_body> request{verb::get, url.Target(), 11};
    request.set(field::host, url.host);
    request.set(field::user_agent, BOOST_BEAST_VERSION_STRING);

    write(socket, request);

    flat_buffer buffer;
    response<dynamic_body> response;

    read(socket, buffer, response);
  });
}

} // namespace react
} // namespace facebook
