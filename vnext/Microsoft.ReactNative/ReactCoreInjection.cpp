// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactCoreInjection.h"
#include "ReactCoreInjection.g.cpp"
#include "ReactViewOptions.g.cpp"
#include <future/futureWinRT.h>
#include "IReactContext.h"
#include "ReactContext.h"
#include "ReactInstanceWin.h"

#include "ReactNativeHost.h"

namespace winrt::Microsoft::ReactNative::implementation {

ReactViewOptions::ReactViewOptions() noexcept {}

winrt::hstring ReactViewOptions::ComponentName() noexcept {
  return m_componentName;
}

void ReactViewOptions::ComponentName(winrt::hstring value) noexcept {
  m_componentName = value;
}

ReactNative::JSValueArgWriter ReactViewOptions::InitialProps() noexcept {
  return m_initialProps;
}

void ReactViewOptions::InitialProps(ReactNative::JSValueArgWriter value) noexcept {
  m_initialProps = value;
}

Mso::React::ReactViewOptions ReactViewOptions::CreateViewOptions() noexcept {
  Mso::React::ReactViewOptions viewOptions;
  viewOptions.ComponentName = winrt::to_string(m_componentName);
  viewOptions.InitialProps = m_initialProps;
  return std::move(viewOptions);
}

ReactCoreInjection::ReactCoreInjection() noexcept {}

static const ReactPropertyId<uint64_t> &TopLevelWindowIdPropertyId() noexcept {
  static const ReactPropertyId<uint64_t> prop{L"ReactNative.Injection", L"TopLevelWindowId"};
  return prop;
}

/*static*/ ReactPropertyId<UIBatchCompleteCallback> ReactCoreInjection::UIBatchCompleteCallbackProperty() noexcept {
  static ReactPropertyId<UIBatchCompleteCallback> prop{L"ReactNative.Injection", L"UIBatchCompleteCallback"};
  return prop;
}

/*static*/ ReactPropertyId<
    winrt::Microsoft::ReactNative::ReactNonAbiValue<std::function<void(ReactNative::ReactDispatcherCallback const &)>>>
ReactCoreInjection::PostToUIBatchingQueueProperty() noexcept {
  static ReactPropertyId<winrt::Microsoft::ReactNative::ReactNonAbiValue<
      std::function<void(ReactNative::ReactDispatcherCallback const &)>>>
      prop{L"ReactNative.Injection", L"PostToUIBatchingQueue"};
  return prop;
}

/*static*/ void ReactCoreInjection::SetUIBatchCompleteCallback(
    IReactPropertyBag const &properties,
    UIBatchCompleteCallback const &callback) noexcept {
  ReactNative::ReactPropertyBag(properties).Set(UIBatchCompleteCallbackProperty(), callback);
}

/*static*/ ReactNative::IReactViewHost ReactCoreInjection::MakeViewHost(
    ReactNative::ReactNativeHost host,
    ReactNative::ReactViewOptions viewOptions) noexcept {
  auto rnhost = host.as<ReactNativeHost>()->ReactHost();
  auto uiDispatcher = host.InstanceSettings()
                          .Properties()
                          .Get(winrt::Microsoft::ReactNative::ReactDispatcherHelper::UIDispatcherProperty())
                          .as<winrt::Microsoft::ReactNative::IReactDispatcher>();
  auto viewHost = rnhost->MakeViewHost(viewOptions.as<ReactViewOptions>()->CreateViewOptions());
  return winrt::make<ReactViewHost>(host, *viewHost, uiDispatcher);
}

/*static*/ void ReactCoreInjection::PostToUIBatchingQueue(
    ReactNative::IReactContext context,
    ReactNative::ReactDispatcherCallback callback) noexcept {
  auto postFn = *ReactNative::ReactPropertyBag(context.Properties()).Get(PostToUIBatchingQueueProperty());
  postFn(callback);
}

ReactPropertyId<winrt::hstring> PlatformNameOverrideProperty() noexcept {
  static ReactPropertyId<winrt::hstring> prop{L"ReactNative.Injection", L"PlatformNameOverride"};
  return prop;
}

/*static*/ void ReactCoreInjection::SetPlatformNameOverride(
    IReactPropertyBag const &properties,
    winrt::hstring const &platformName) noexcept {
  ReactNative::ReactPropertyBag(properties).Set(PlatformNameOverrideProperty(), platformName);
}
/*static*/ std::string ReactCoreInjection::GetPlatformName(IReactPropertyBag const &properties) noexcept {
  return winrt::to_string(ReactNative::ReactPropertyBag(properties)
                              .Get(PlatformNameOverrideProperty())
                              .value_or(winrt::to_hstring(STRING(RN_PLATFORM))));
}

uint64_t ReactCoreInjection::GetTopLevelWindowId(const IReactPropertyBag &properties) noexcept {
  return ReactPropertyBag(properties).Get(TopLevelWindowIdPropertyId()).value_or(0);
}

void ReactCoreInjection::SetTopLevelWindowId(const IReactPropertyBag &properties, uint64_t windowId) noexcept {
  ReactPropertyBag(properties).Set(TopLevelWindowIdPropertyId(), windowId);
}

ReactViewHost::ReactViewHost(
    const ReactNative::ReactNativeHost &host,
    Mso::React::IReactViewHost &viewHost,
    const winrt::Microsoft::ReactNative::IReactDispatcher &uiDispatcher)
    : m_host(host), m_viewHost(&viewHost), m_uiDispatcher(uiDispatcher) {}

// ReactViewOptions ReactViewHost::Options() noexcept;
// ReactNative::ReactNativeHost ReactViewHost::ReactHost() noexcept {}
winrt::Windows::Foundation::IAsyncAction ReactViewHost::ReloadViewInstance() noexcept {
  return make<Mso::AsyncActionFutureAdapter>(m_viewHost->ReloadViewInstance());
}

winrt::Windows::Foundation::IAsyncAction ReactViewHost::ReloadViewInstanceWithOptions(
    ReactNative::ReactViewOptions options) noexcept {
  return make<Mso::AsyncActionFutureAdapter>(
      m_viewHost->ReloadViewInstanceWithOptions(options.as<ReactViewOptions>()->CreateViewOptions()));
}

winrt::Windows::Foundation::IAsyncAction ReactViewHost::UnloadViewInstance() noexcept {
  return make<Mso::AsyncActionFutureAdapter>(m_viewHost->UnloadViewInstance());
}

//! This class ensures that we access ReactRootView from UI thread.
struct ReactViewInstance : public Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, Mso::React::IReactViewInstance> {
  ReactViewInstance(
      const ReactNative::IReactViewInstance &rootControl,
      const winrt::Microsoft::ReactNative::IReactDispatcher &uiDispatcher) noexcept
      : m_rootControl{rootControl}, m_uiDispatcher{uiDispatcher} {}

  Mso::Future<void> InitRootView(
      Mso::CntPtr<Mso::React::IReactInstance> &&reactInstance,
      Mso::React::ReactViewOptions &&viewOptions) noexcept override {
    return PostInUIQueue([reactInstance{std::move(reactInstance)}, viewOptions{std::move(viewOptions)}](
                             ReactNative::IReactViewInstance rootControl) mutable noexcept {
      auto pinstanceWin = static_cast<Mso::React::ReactInstanceWin *>(reactInstance.Get());
      Mso::CntPtr<Mso::React::IReactContext> context = &(pinstanceWin->GetReactContext());

      ReactNative::ReactViewOptions options;
      options.ComponentName(winrt::to_hstring(viewOptions.ComponentName));
      options.InitialProps(viewOptions.InitialProps);

      rootControl.InitRootView(winrt::make<ReactContext>(std::move(context)), options);
    });
  }
  Mso::Future<void> UpdateRootView() noexcept override {
    return PostInUIQueue(
        [](ReactNative::IReactViewInstance rootControl) mutable noexcept { rootControl.UpdateRootView(); });
  }
  Mso::Future<void> UninitRootView() noexcept override {
    return PostInUIQueue(
        [](ReactNative::IReactViewInstance rootControl) mutable noexcept { rootControl.UninitRootView(); });
  }

 private:
  winrt::Microsoft::ReactNative::IReactViewInstance m_rootControl;
  winrt::Microsoft::ReactNative::IReactDispatcher m_uiDispatcher;

  inline Mso::Future<void> PostInUIQueue(winrt::delegate<ReactNative::IReactViewInstance> const &action) noexcept {
    Mso::Promise<void> promise;

    m_uiDispatcher.Post([control = m_rootControl, promise, action{std::move(action)}]() mutable noexcept {
      action(control);
      promise.SetValue();
    });
    return promise.AsFuture();
  }
};

winrt::Windows::Foundation::IAsyncAction ReactViewHost::AttachViewInstance(
    ReactNative::IReactViewInstance viewInstance) noexcept {
  return make<Mso::AsyncActionFutureAdapter>(
      m_viewHost->AttachViewInstance(*Mso::Make<ReactViewInstance>(viewInstance, m_uiDispatcher)));
}

winrt::Windows::Foundation::IAsyncAction ReactViewHost::DetachViewInstance() noexcept {
  return make<Mso::AsyncActionFutureAdapter>(m_viewHost->DetachViewInstance());
}

winrt::Microsoft::ReactNative::ReactNativeHost ReactViewHost::ReactNativeHost() noexcept {
  return m_host;
}

} // namespace winrt::Microsoft::ReactNative::implementation
