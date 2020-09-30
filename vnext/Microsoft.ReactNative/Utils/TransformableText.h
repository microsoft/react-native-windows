// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

namespace react::uwp {
struct TransformableText final {
  enum class TextTransform : uint8_t { Undefined, None, Uppercase, Lowercase, Capitalize };

  TextTransform textTransform{TextTransform::Undefined};
  std::wstring originalText{};

  std::wstring TransformText() const noexcept {
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
        LOCALE_NAME_USER_DEFAULT,
        dwMapFlags,
        originalText.c_str(),
        static_cast<int>(originalText.length()),
        nullptr,
        0);

    std::wstring str;
    str.resize(reqChars);
    const int nChars = LCMapStringW(
        LOCALE_NAME_USER_DEFAULT,
        dwMapFlags,
        originalText.c_str(),
        static_cast<int>(originalText.length()),
        str.data(),
        reqChars);
    str.resize(nChars);
    assert(nChars == reqChars);
    return str;
  }

  static TextTransform GetTextTransform(const folly::dynamic &propertyValue) noexcept {
    if (propertyValue.isString()) {
      auto value = propertyValue.asString();
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

} // namespace react::uwp
