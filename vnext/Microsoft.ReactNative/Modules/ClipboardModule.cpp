// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ClipboardModule.h"
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include "Unicode.h"

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::ApplicationModel;

namespace Microsoft::ReactNative {

void Clipboard::Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  m_reactContext = reactContext;
}

void Clipboard::getString(React::ReactPromise<React::JSValue> result) noexcept {
  auto jsDispatcher = m_reactContext.JSDispatcher();
  m_reactContext.UIDispatcher().Post([jsDispatcher, result] {
    auto data = DataTransfer::Clipboard::GetContent();
    auto asyncOp = data.GetTextAsync();
    // unfortunately, lambda captures doesn't work well with winrt::fire_and_forget and co_await here
    // call asyncOp.Completed explicitly
    asyncOp.Completed([jsDispatcher, result](const IAsyncOperation<winrt::hstring> &asyncOp, AsyncStatus status) {
      switch (status) {
        case AsyncStatus::Completed: {
          auto text = std::wstring(asyncOp.GetResults());
          jsDispatcher.Post(
              [result, text] { result.Resolve(React::JSValue{Microsoft::Common::Unicode::Utf16ToUtf8(text)}); });
          break;
        }
        case AsyncStatus::Canceled: {
          jsDispatcher.Post([result] { result.Reject(React::ReactError()); });
          break;
        }
        case AsyncStatus::Error: {
          auto message = std::wstring(winrt::hresult_error(asyncOp.ErrorCode()).message());
          jsDispatcher.Post([result, message] { result.Reject(message.c_str()); });
          break;
        }
        case AsyncStatus::Started: {
          assert(false); // should never happen
        }
      }
    });
  } // namespace Microsoft::ReactNative
  );
}

void Clipboard::setString(std::string content) noexcept {
  m_reactContext.UIDispatcher().Post([=] {
    DataTransfer::DataPackage data;
    data.SetText(Microsoft::Common::Unicode::Utf8ToUtf16(content));
    DataTransfer::Clipboard::SetContent(data);
  });
}

} // namespace Microsoft::ReactNative
