// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ClipboardModule.h"

#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include "Unicode.h"

#pragma warning(push)
#pragma warning(disable : 4146)
#include <cxxreact/JsArgumentHelpers.h>
#pragma warning(pop)

#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize("", off)
#endif

namespace react {
namespace uwp {

//
// ClipboardModule
//
const char *ClipboardModule::name = "Clipboard";

ClipboardModule::~ClipboardModule() = default;

std::string ClipboardModule::getName() {
  return name;
}

std::map<std::string, folly::dynamic> ClipboardModule::getConstants() {
  return {};
}

std::vector<facebook::xplat::module::CxxModule::Method>
ClipboardModule::getMethods() {
  return {
      Method(
          "setString",
          [](folly::dynamic args) {
            ClipboardModule::SetClipboardText(
                facebook::xplat::jsArgAsString(args, 0));
          }),
      Method(
          "getString",
          [](folly::dynamic args, Callback cbSuccess, Callback cbFail) {
            ClipboardModule::GetClipboardText(cbSuccess, cbFail);
          }),

  };
}

/*static*/ void ClipboardModule::SetClipboardText(const std::string &text) {
  winrt::Windows::ApplicationModel::DataTransfer::DataPackage data;
  data.SetText(facebook::react::unicode::utf8ToUtf16(text));
  winrt::Windows::ApplicationModel::DataTransfer::Clipboard::SetContent(data);
}

winrt::fire_and_forget GetClipboardTextAsync(
    facebook::xplat::module::CxxModule::Callback cbSuccess,
    facebook::xplat::module::CxxModule::Callback cbFail) {
  winrt::Windows::ApplicationModel::DataTransfer::DataPackageView data =
      winrt::Windows::ApplicationModel::DataTransfer::Clipboard::GetContent();
  try {
    std::wstring text = std::wstring(co_await data.GetTextAsync());
    cbSuccess({facebook::react::unicode::utf16ToUtf8(text)});
  } catch (...) {
    cbFail({});
  }
}

/*static*/ void ClipboardModule::GetClipboardText(
    const Callback &cbSuccess,
    const Callback &cbFail) {
  GetClipboardTextAsync(cbSuccess, cbFail);
}

} // namespace uwp
} // namespace react
