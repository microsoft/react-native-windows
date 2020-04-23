// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ClipboardModule.h"
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include "Unicode.h"

namespace Microsoft::ReactNative {

/* static */ winrt::fire_and_forget Clipboard::getString(React::ReactPromise<React::JSValue> result) noexcept {
  winrt::Windows::ApplicationModel::DataTransfer::DataPackageView data =
      winrt::Windows::ApplicationModel::DataTransfer::Clipboard::GetContent();
  try {
    std::wstring text = std::wstring(co_await data.GetTextAsync());
    result.Resolve(React::JSValue{Microsoft::Common::Unicode::Utf16ToUtf8(text)});
  } catch (winrt::hresult_error const &e) {
    result.Reject(std::wstring(e.message()).c_str());
  } catch (...) {
    result.Reject(React::ReactError());
  }
}

/* static */ void Clipboard::setString(std::string content) noexcept {
  winrt::Windows::ApplicationModel::DataTransfer::DataPackage data;
  data.SetText(Microsoft::Common::Unicode::Utf8ToUtf16(content));
  winrt::Windows::ApplicationModel::DataTransfer::Clipboard::SetContent(data);
}

} // namespace Microsoft::ReactNative
