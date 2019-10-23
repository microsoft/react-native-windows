// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "WebViewManager.h"

#include <Views/ShadowNodeBase.h>

#include <Utils/ValueUtils.h>

#include <IReactInstance.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

#include <condition_variable>
#include <mutex>
#include <type_traits>

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml::Controls;
} // namespace winrt

namespace react {
namespace uwp {

WebViewManager::WebViewManager(const std::shared_ptr<IReactInstance> &reactInstance) : Super(reactInstance) {
  SetupPropertyHandlersInternal();
}

const char *WebViewManager::GetName() const {
  return "RCTWebView";
}

XamlView WebViewManager::CreateViewCore(int64_t tag) {
#ifdef CHAKRACORE_UWP
  return winrt::WebView(winrt::WebViewExecutionMode::SeparateProcess);
#else
  return winrt::WebView();
#endif
}

void WebViewManager::UpdateProperties(ShadowNodeBase *nodeToUpdate, const folly::dynamic &reactDiffMap) {
  XamlView view = nodeToUpdate->GetView();
  UpdatePropertiesInternal(view, reactDiffMap);
  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

void WebViewManager::setSource(XamlView viewToUpdate, const WebSource &source) {
  auto instance = m_wkReactInstance.lock();
  if (instance == nullptr)
    return;
  auto view = viewToUpdate.as<winrt::WebView>();

  // non-uri sources not yet supported
  if (source.uri.length() == 0)
    return;

  auto uriString = source.uri;
  if (source.packagerAsset && uriString.find("assets") == 0)
    uriString.replace(0, 6, "ms-appx://");

  auto uri = winrt::Uri(winrt::hstring(asWStr(uriString).c_str()));

  auto tag = GetTag(view);
  folly::dynamic eventData = folly::dynamic::object("target", tag);
  instance->DispatchEvent(tag, "topLoadStart", std::move(eventData));

  view.Navigate(uri);
}

folly::dynamic WebViewManager::GetExportedCustomDirectEventTypeConstants() const {
  auto directEvents = Super::GetExportedCustomDirectEventTypeConstants();
  directEvents["topLoadStart"] = folly::dynamic::object("registrationName", "onLoadStart");
  directEvents["topLoad"] = folly::dynamic::object("registrationName", "onLoad");
  directEvents["topLoadEnd"] = folly::dynamic::object("registrationName", "onLoadEnd");

  return directEvents;
}

folly::dynamic WebViewManager::GetNativeProps() const {
  auto props = Super::GetNativeProps();
  // TODO: implement native props propagation from property map
  props.update(folly::dynamic::object("source", "Map"));

  return props;
}

} // namespace uwp
} // namespace react
