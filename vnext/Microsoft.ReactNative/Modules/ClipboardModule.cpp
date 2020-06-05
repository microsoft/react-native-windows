// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ClipboardModule.h"
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include "Unicode.h"

namespace Microsoft::ReactNative {

void Clipboard::Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  m_reactContext = reactContext;
}

void Clipboard::getString(React::ReactPromise<React::JSValue> result) noexcept {
  auto jsDispatcher = m_reactContext.JSDispatcher();
  auto promise = result;
  m_reactContext.UIDispatcher().Post(
      [=]() -> winrt::fire_and_forget {
        auto jsDispatcher2 = jsDispatcher;
        auto promise2 = promise;
        winrt::Windows::ApplicationModel::DataTransfer::DataPackageView data =
            winrt::Windows::ApplicationModel::DataTransfer::Clipboard::GetContent();
        try {
          std::wstring text = std::wstring(co_await data.GetTextAsync());
          jsDispatcher2.Post([=] { promise2.Resolve(React::JSValue{Microsoft::Common::Unicode::Utf16ToUtf8(text)}); });
        } catch (winrt::hresult_error const &e) {
          jsDispatcher2.Post([=] { promise2.Reject(std::wstring(e.message()).c_str()); });
        } catch (...) {
          jsDispatcher2.Post([=] { promise2.Reject(React::ReactError()); });
        }
      } // namespace Microsoft::ReactNative
  );
}

void Clipboard::setString(std::string content) noexcept {
  m_reactContext.UIDispatcher().Post([=] {
    winrt::Windows::ApplicationModel::DataTransfer::DataPackage data;
    data.SetText(Microsoft::Common::Unicode::Utf8ToUtf16(content));
    winrt::Windows::ApplicationModel::DataTransfer::Clipboard::SetContent(data);
  });
}

} // namespace Microsoft::ReactNative
