// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "UwpReactInstanceProxy.h"

#include <ReactHost/React_Win.h>
#include <crash/verifyElseCrash.h>

namespace react::uwp {

UwpReactInstanceProxy::UwpReactInstanceProxy(Mso::WeakPtr<Mso::React::IReactInstance> &&weakReactInstance) noexcept
    : m_weakReactInstance{weakReactInstance} {}

void UwpReactInstanceProxy::AttachMeasuredRootView(IXamlRootView * /*pRootView*/, folly::dynamic && /*initProps*/) {
  VerifyElseCrashSz(false, "Deprecated. Use ReactHost.");
}

void UwpReactInstanceProxy::DetachRootView(IXamlRootView * /*pRootView*/) {
  VerifyElseCrashSz(false, "Deprecated. Use ReactHost.");
}

LiveReloadCallbackCookie UwpReactInstanceProxy::RegisterLiveReloadCallback(std::function<void()> /*callback*/) {
  VerifyElseCrashSz(false, "Deprecated. Use ReactHost.");
}

void UwpReactInstanceProxy::UnregisterLiveReloadCallback(LiveReloadCallbackCookie & /*cookie*/) {
  VerifyElseCrashSz(false, "Deprecated. Use ReactHost.");
}

ErrorCallbackCookie UwpReactInstanceProxy::RegisterErrorCallback(std::function<void()> /*callback*/) {
  VerifyElseCrashSz(false, "Deprecated. Use ReactHost.");
}

void UwpReactInstanceProxy::UnregisterErrorCallback(ErrorCallbackCookie & /*cookie*/) {
  VerifyElseCrashSz(false, "Deprecated. Use ReactHost.");
}

DebuggerAttachCallbackCookie UwpReactInstanceProxy::RegisterDebuggerAttachCallback(std::function<void()> /*callback*/) {
  VerifyElseCrashSz(false, "Deprecated. Use ReactHost.");
}

void UwpReactInstanceProxy::UnRegisterDebuggerAttachCallback(DebuggerAttachCallbackCookie & /*cookie*/) {
  VerifyElseCrashSz(false, "Deprecated. Use ReactHost.");
}

void UwpReactInstanceProxy::DispatchEvent(int64_t viewTag, std::string eventName, folly::dynamic &&eventData) {
  if (auto reactInstance = m_weakReactInstance.GetStrongPtr()) {
    query_cast<Mso::React::ILegacyReactInstance &>(*reactInstance)
        .DispatchEvent(viewTag, std::move(eventName), std::move(eventData));
  }
}

void UwpReactInstanceProxy::CallJsFunction(
    std::string &&moduleName,
    std::string &&method,
    folly::dynamic &&params) noexcept {
  if (auto reactInstance = m_weakReactInstance.GetStrongPtr()) {
    query_cast<Mso::React::ILegacyReactInstance &>(*reactInstance)
        .CallJsFunction(std::move(moduleName), std::move(method), std::move(params));
  }
}

const std::shared_ptr<facebook::react::MessageQueueThread> &UwpReactInstanceProxy::JSMessageQueueThread() const
    noexcept {
  VerifyElseCrashSz(false, "Deprecated. Use ReactHost.");
}

const std::shared_ptr<facebook::react::MessageQueueThread> &UwpReactInstanceProxy::DefaultNativeMessageQueueThread()
    const noexcept {
  VerifyElseCrashSz(false, "Deprecated. Use ReactHost.");
}

facebook::react::INativeUIManager *UwpReactInstanceProxy::NativeUIManager() const noexcept {
  if (auto reactInstance = m_weakReactInstance.GetStrongPtr()) {
    return query_cast<Mso::React::ILegacyReactInstance &>(*reactInstance).NativeUIManager();
  }

  return nullptr;
}

bool UwpReactInstanceProxy::NeedsReload() const noexcept {
  VerifyElseCrashSz(false, "Deprecated. Use ReactHost.");
}

void UwpReactInstanceProxy::SetAsNeedsReload() noexcept {
  VerifyElseCrashSz(false, "Deprecated. Use ReactHost.");
}

std::shared_ptr<facebook::react::Instance> UwpReactInstanceProxy::GetInnerInstance() const noexcept {
  if (auto reactInstance = m_weakReactInstance.GetStrongPtr()) {
    return query_cast<Mso::React::ILegacyReactInstance &>(*reactInstance).GetInnerInstance();
  }

  return nullptr;
}

bool UwpReactInstanceProxy::IsInError() const noexcept {
  if (auto reactInstance = m_weakReactInstance.GetStrongPtr()) {
    return reactInstance->State() == Mso::React::ReactInstanceState::HasError;
  }

  return true;
}

bool UwpReactInstanceProxy::IsWaitingForDebugger() const noexcept {
  if (auto reactInstance = m_weakReactInstance.GetStrongPtr()) {
    return reactInstance->State() == Mso::React::ReactInstanceState::WaitingForDebugger;
  }

  return false;
}

const std::string &UwpReactInstanceProxy::LastErrorMessage() const noexcept {
  VerifyElseCrashSz(false, "Deprecated. Use ReactHost.");
}

void UwpReactInstanceProxy::loadBundle(std::string && /*jsBundleRelativePath*/) {
  VerifyElseCrashSz(false, "Deprecated. Use ReactHost.");
}

ExpressionAnimationStore &UwpReactInstanceProxy::GetExpressionAnimationStore() {
  return m_expressionAnimationStore;
}

std::string UwpReactInstanceProxy::GetBundleRootPath() const noexcept {
  if (auto reactInstance = m_weakReactInstance.GetStrongPtr()) {
    return query_cast<Mso::React::ILegacyReactInstance &>(*reactInstance).GetBundleRootPath();
  }

  return "";
}

bool UwpReactInstanceProxy::IsLoaded() const noexcept {
  if (auto reactInstance = m_weakReactInstance.GetStrongPtr()) {
    return query_cast<Mso::React::ILegacyReactInstance &>(*reactInstance).IsLoaded();
  }

  return false;
}

void UwpReactInstanceProxy::SetXamlViewCreatedTestHook(std::function<void(react::uwp::XamlView)> testHook) {
  m_xamlViewCreatedTestHook = std::move(testHook);
}

void UwpReactInstanceProxy::CallXamlViewCreatedTestHook(react::uwp::XamlView view) {
  if (m_xamlViewCreatedTestHook != nullptr) {
    m_xamlViewCreatedTestHook(view);
  }
}

} // namespace react::uwp
