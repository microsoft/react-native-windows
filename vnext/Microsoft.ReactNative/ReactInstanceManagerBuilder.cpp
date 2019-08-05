// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactInstanceManagerBuilder.h"

using namespace winrt;
using namespace Microsoft::ReactNative;

namespace winrt::Microsoft::ReactNative::implementation {

winrt::Microsoft::ReactNative::ReactInstanceManager
ReactInstanceManagerBuilder::Build() {
  if (!m_isLifecycleStateSet) {
    throw hresult_invalid_operation(
        L"Initial lifecycle state was not set.");
  }

  if (!m_useDeveloperSupport && m_jsBundleFile.empty()) {
    throw hresult_invalid_operation(
        L"JavaScriptBundleFile must be provided when UseDeveloperSupport is false.");
  }

  if (m_jsBundleFile.empty() && m_jsMainModuleName.empty()) {
    throw hresult_invalid_operation(
        L"Either MainModulePath or the JavaScriptBundleFile must be provided.");
  }

  auto instanceManager = make<ReactInstanceManager>(
      m_instanceSettings,
      to_string(m_jsBundleFile),
      to_string(m_jsMainModuleName),
      m_packages,
      m_useDeveloperSupport,
      m_initialLifecycleState);
  return instanceManager;
}
} // namespace winrt::Microsoft::ReactNative::implementation
