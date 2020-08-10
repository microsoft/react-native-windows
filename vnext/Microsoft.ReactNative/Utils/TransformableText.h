#pragma once

#include <boost/algorithm/string.hpp>

namespace react::uwp {
struct TransformableText final {
  enum class TextTransform { Undefined, None, Uppercase, Lowercase, Capitalize };

  TextTransform m_textTransform{TextTransform::Undefined};
  std::wstring m_originalText{};

  std::wstring TransformText() {
    switch (m_textTransform) {
      case TextTransform::Undefined:
        [[fallthrough]];
      case TextTransform::None:
        return m_originalText;
      case TextTransform::Uppercase:
        return boost::to_upper_copy(m_originalText);
      case TextTransform::Lowercase:
        return boost::to_lower_copy(m_originalText);
      case TextTransform::Capitalize: {
        auto str = std::make_unique<wchar_t[]>(m_originalText.length() + 1);
        const int nChars = LCMapStringW(
            LOCALE_NAME_USER_DEFAULT,
            LCMAP_TITLECASE,
            m_originalText.c_str(),
            static_cast<int>(m_originalText.length()),
            str.get(),
            static_cast<int>(m_originalText.length() + 1));

        return std::move(str.get());
      }
    }
    nyi();
    return m_originalText;
  }

  static TextTransform GetTextTransform(const folly::dynamic &propertyValue) {
    if (propertyValue.isString()) {
      auto value = propertyValue.asString();
      if (value == "none") {
      } else if (value == "lowercase") {
        return TextTransform::Lowercase;
      } else if (value == "uppercase") {
        return TextTransform ::Uppercase;
      } else if (value == "capitalize") {
        return TextTransform::Capitalize;
      }
    }
    return TextTransform::None;
  }
};

} // namespace react::uwp
