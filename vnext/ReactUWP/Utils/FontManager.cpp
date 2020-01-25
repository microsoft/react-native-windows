// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "Utils/FontManager.h"

namespace react {
namespace uwp {

void FontManager::SetFont(
    const std::wstring &fontFamily,
    winrt::Windows::UI::Text::FontWeight weight,
    winrt::Windows::UI::Text::FontStyle style,
    const std::wstring &filePath) {
  FontKey f{fontFamily, weight.Weight, static_cast<int32_t>(style)};

  m_fonts.emplace(f, winrt::Windows::UI::Xaml::Media::FontFamily(filePath));
}

winrt::Windows::UI::Xaml::Media::FontFamily FontManager::GetFont(
    const std::wstring &fontFamily,
    winrt::Windows::UI::Text::FontWeight weight,
    winrt::Windows::UI::Text::FontStyle style) {
  FontKey f{fontFamily, weight.Weight, static_cast<int32_t>(style)};

  // Use exact match if available
  auto iter = m_fonts.find(f);
  if (iter != m_fonts.end()) {
    return iter->second;
  }

  // If available, fall back to the normal font in the family
  f.weight = winrt::Windows::UI::Text::FontWeights::Normal().Weight;
  f.style = static_cast<int32_t>(winrt::Windows::UI::Text::FontStyle::Normal);

  iter = m_fonts.find(f);
  if (iter != m_fonts.end()) {
    return iter->second;
  }

  // Fall back to system font

  // FontFamily ctor throws if name is empty, so use a default
  if (fontFamily.size() == 0) {
    return winrt::Windows::UI::Xaml::Media::FontFamily::XamlAutoFontFamily();
  }

  return winrt::Windows::UI::Xaml::Media::FontFamily(fontFamily);
}

} // namespace uwp
} // namespace react
