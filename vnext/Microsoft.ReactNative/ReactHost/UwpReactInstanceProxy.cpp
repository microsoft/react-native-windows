// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "UwpReactInstanceProxy.h"
#include <ReactHost/React_Win.h>

namespace react::uwp {

UwpReactInstanceProxy::UwpReactInstanceProxy(Mso::CntPtr<Mso::React::IReactContext> &&context) noexcept
    : m_context{std::move(context)} {}

void UwpReactInstanceProxy::DispatchEvent(
    int64_t viewTag,
    std::string &&eventName,
    folly::dynamic &&eventData) noexcept {
  m_context->DispatchEvent(viewTag, std::move(eventName), std::move(eventData));
}

void UwpReactInstanceProxy::CallJsFunction(
    std::string &&moduleName,
    std::string &&method,
    folly::dynamic &&params) noexcept {
  m_context->CallJSFunction(std::move(moduleName), std::move(method), std::move(params));
}

facebook::react::INativeUIManager *UwpReactInstanceProxy::NativeUIManager() const noexcept {
  return m_context->NativeUIManager();
}

std::shared_ptr<facebook::react::Instance> UwpReactInstanceProxy::GetInnerInstance() const noexcept {
  return m_context->GetInnerInstance();
}

bool UwpReactInstanceProxy::IsInError() const noexcept {
  return m_context->State() == Mso::React::ReactInstanceState::HasError;
}

bool UwpReactInstanceProxy::IsWaitingForDebugger() const noexcept {
  return m_context->State() == Mso::React::ReactInstanceState::WaitingForDebugger;
}

ExpressionAnimationStore &UwpReactInstanceProxy::GetExpressionAnimationStore() noexcept {
  return m_expressionAnimationStore;
}

std::string UwpReactInstanceProxy::GetBundleRootPath() const noexcept {
  return m_context->GetBundleRootPath();
}

bool UwpReactInstanceProxy::IsLoaded() const noexcept {
  return m_context->IsLoaded();
}

void UwpReactInstanceProxy::SetXamlViewCreatedTestHook(std::function<void(react::uwp::XamlView)> &&testHook) noexcept {
  m_xamlViewCreatedTestHook = std::move(testHook);
}

void UwpReactInstanceProxy::CallXamlViewCreatedTestHook(react::uwp::XamlView const &view) noexcept {
  if (m_xamlViewCreatedTestHook != nullptr) {
    m_xamlViewCreatedTestHook(view);
  }
}

} // namespace react::uwp
