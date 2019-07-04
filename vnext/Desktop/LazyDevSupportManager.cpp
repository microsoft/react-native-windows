// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "LazyDevSupportManager.h"

using std::string;

namespace facebook {
namespace react {

void LazyDevSupportManager::EnsureDevSupportManager() noexcept {
  if (!m_dsm)
    m_dsm = std::make_unique<DevSupportManager>();
}

#pragma region LazyDevSupportManager

JSECreator LazyDevSupportManager::LoadJavaScriptInProxyMode(
    const DevSettings &settings) /*override*/
{
  EnsureDevSupportManager();

  return m_dsm->LoadJavaScriptInProxyMode(settings);
}

#if !defined(OSS_RN)
JSECreator LazyDevSupportManager::LoadJavaScriptInSandboxMode(
    const DevSettings &settings) /*override*/
{
  EnsureDevSupportManager();

  return m_dsm->LoadJavaScriptInSandboxMode(settings);
}
#endif

string LazyDevSupportManager::GetJavaScriptFromServer(
    const string &debugHost,
    const string &jsBundleName,
    const string &platform) /*override*/
{
  EnsureDevSupportManager();

  return m_dsm->GetJavaScriptFromServer(debugHost, jsBundleName, platform);
}

void LazyDevSupportManager::StartPollingLiveReload(
    const string &debugHost,
    std::function<void()> onChangeCallback) /*override*/
{
  EnsureDevSupportManager();

  m_dsm->StartPollingLiveReload(debugHost, onChangeCallback);
}

void LazyDevSupportManager::StopPollingLiveReload() /*override*/
{
  EnsureDevSupportManager();

  m_dsm->StopPollingLiveReload();
}

bool LazyDevSupportManager::HasException() /*override*/
{
  EnsureDevSupportManager();

  return m_dsm->HasException();
}

#pragma endregion LazyDevSupportManager

std::shared_ptr<IDevSupportManager> CreateDevSupportManager() {
  return std::make_shared<LazyDevSupportManager>();
}

} // namespace react
} // namespace facebook
