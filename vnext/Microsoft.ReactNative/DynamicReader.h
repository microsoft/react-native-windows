// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "folly/dynamic.h"
#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative {

struct DynamicReader : implements<DynamicReader, IJSValueReader> {
  DynamicReader(const folly::dynamic &root) noexcept;

 public: // IJSValueReader
  JSValueType ValueType() noexcept;
  bool GetNextObjectProperty(hstring &propertyName) noexcept;
  bool GetNextArrayItem() noexcept;
  hstring GetString() noexcept;
  bool GetBoolean() noexcept;
  int64_t GetInt64() noexcept;
  double GetDouble() noexcept;

 private:
  struct StackEntry {
    static StackEntry ObjectProperty(
        const folly::dynamic *value,
        const folly::dynamic::const_item_iterator &property) noexcept;
    static StackEntry ArrayItem(const folly::dynamic *value, const folly::dynamic::const_iterator &item) noexcept;

    const folly::dynamic *Value{nullptr};
    folly::dynamic::const_iterator Item;
    std::optional<folly::dynamic::const_item_iterator> Property;
  };

 private:
  void SetCurrentValue(const folly::dynamic *value) noexcept;

 private:
  const folly::dynamic *m_current{nullptr};
  bool m_isIterating{false};
  std::vector<StackEntry> m_stack;
};

} // namespace winrt::Microsoft::ReactNative
