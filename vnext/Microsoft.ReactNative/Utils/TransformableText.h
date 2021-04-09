// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Utils/TextTransform.h>

namespace Microsoft::ReactNative {
struct TransformableText final {
  static winrt::hstring TransformText(const winrt::hstring &originalText, TextTransform textTransform) noexcept {
    if (textTransform == TextTransform::Undefined || textTransform == TextTransform::None) {
      return originalText;
    }

    DWORD dwMapFlags{};
    switch (textTransform) {
      case TextTransform::Uppercase:
        dwMapFlags |= LCMAP_UPPERCASE | LCMAP_LINGUISTIC_CASING;
        break;
      case TextTransform::Lowercase:
        dwMapFlags |= LCMAP_LOWERCASE | LCMAP_LINGUISTIC_CASING;
        break;
      case TextTransform::Capitalize:
        dwMapFlags |= LCMAP_TITLECASE;
        break;
      default:
        nyi();
        return originalText;
    }

    const int reqChars = LCMapStringW(
        LOCALE_NAME_USER_DEFAULT, dwMapFlags, originalText.c_str(), static_cast<int>(originalText.size()), nullptr, 0);

    std::wstring str;
    str.resize(reqChars);
    const int nChars = LCMapStringW(
        LOCALE_NAME_USER_DEFAULT,
        dwMapFlags,
        originalText.c_str(),
        static_cast<int>(originalText.size()),
        str.data(),
        reqChars);
    str.resize(nChars);
    assert(nChars == reqChars);
    winrt::hstring result{str.c_str()};
    return result;
  }

  static TextTransform GetTextTransform(const winrt::Microsoft::ReactNative::JSValue &propertyValue) noexcept {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
      auto value = propertyValue.AsString();
      if (value == "none") {
        return TextTransform::None;
      } else if (value == "lowercase") {
        return TextTransform::Lowercase;
      } else if (value == "uppercase") {
        return TextTransform ::Uppercase;
      } else if (value == "capitalize") {
        return TextTransform::Capitalize;
      }
    }
    return TextTransform::Undefined;
  }
};

} // namespace Microsoft::ReactNative
