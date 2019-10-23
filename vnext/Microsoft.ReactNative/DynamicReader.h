#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "folly/dynamic.h"
#include "winrt/Microsoft.ReactNative.Bridge.h"

namespace winrt::Microsoft::ReactNative::Bridge {

struct DynamicReader : winrt::implements<DynamicReader, winrt::Microsoft::ReactNative::Bridge::IJSValueReader> {
  DynamicReader(const folly::dynamic &root) noexcept;

 public: // IJSValueReader
  winrt::Microsoft::ReactNative::Bridge::JSValueReaderState ReadNext() noexcept;
  _Success_(return ) bool TryGetBoolen(_Out_ bool &value) noexcept;
  _Success_(return ) bool TryGetInt64(_Out_ int64_t &value) noexcept;
  _Success_(return ) bool TryGetDouble(_Out_ double &value) noexcept;
  _Success_(return ) bool TryGetString(winrt::hstring &value) noexcept;

 private:
  struct StackEntry {
    StackEntry(const folly::dynamic *value) noexcept : Value{value} {}

    ~StackEntry() noexcept {}

    const folly::dynamic *Value{nullptr};
    folly::dynamic::const_iterator Item;
    std::optional<folly::dynamic::const_item_iterator> Property;
  };

 private:
  winrt::Microsoft::ReactNative::Bridge::JSValueReaderState ReadValue(const folly::dynamic *value) noexcept;
  static std::u16string Utf8ToUtf16(const char *value, size_t size) noexcept;

 private:
  const folly::dynamic *m_root;
  winrt::Microsoft::ReactNative::Bridge::JSValueReaderState m_state{
      winrt::Microsoft::ReactNative::Bridge::JSValueReaderState::Error};
  std::vector<StackEntry> m_stack;
  std::u16string m_u16str; // Used for strings converted to UTF16
};

} // namespace winrt::Microsoft::ReactNative::Bridge
