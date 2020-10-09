// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CreateModules.h>
#include <Modules/WebSocketModule.h>
#include <Modules/WebSocketModuleUwp.h>
#include <QuirkSettings.h>
#include <React.h>
#include <ReactPropertyBag.h>
#include <WinRTWebSocketResource.h>

// React Native
#include <cxxreact/CxxModule.h>

// Windows API
#include <winrt/Windows.Security.Cryptography.Certificates.h>

using winrt::Microsoft::ReactNative::ReactPropertyBag;
using winrt::Microsoft::ReactNative::ReactPropertyId;
using winrt::Microsoft::ReactNative::implementation::QuirkSettings;

namespace Microsoft::React {

std::shared_ptr<IWebSocketResource> IWebSocketResource::Make(std::string &&urlString) {
  std::vector<winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult> certExceptions;
  return std::make_shared<WinRTWebSocketResource>(std::move(urlString), std::move(certExceptions));
}

std::unique_ptr<facebook::xplat::module::CxxModule> CreateWebSocketModule(
    Mso::CntPtr<Mso::React::IReactContext> &&context) noexcept {
  if (context && QuirkSettings::GetUseLegacyWebSocketModule(ReactPropertyBag(context->Properties()))) {
    return std::make_unique<react::uwp::LegacyWebSocketModule>();
  }
  return std::make_unique<WebSocketModule>();
}

} // namespace Microsoft::React
