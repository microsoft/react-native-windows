// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "Utils/FontManager.h"

namespace react {
namespace uwp {

FontManager FontManager::getInstance() {
  static FontManager s_fontManager;
  return s_fontManager;
}

void FontManager::SetFont(
    const std::wstring &fontFamilyName,
    winrt::Windows::UI::Text::FontWeight weight,
    winrt::Windows::UI::Text::FontStyle style,
    const std::wstring &filePath) {
  FontKey f{fontFamilyName, weight.Weight, static_cast<int32_t>(style)};

  m_fonts.emplace(f, winrt::Windows::UI::Xaml::Media::FontFamily(filePath));
}

winrt::Windows::UI::Xaml::Media::FontFamily FontManager::GetFont(
    const std::wstring &fontFamilyName,
    winrt::Windows::UI::Text::FontWeight weight,
    winrt::Windows::UI::Text::FontStyle style) {
  FontKey f{fontFamilyName, weight.Weight, static_cast<int32_t>(style)};

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
  if (fontFamilyName.size() == 0) {
    return winrt::Windows::UI::Xaml::Media::FontFamily::XamlAutoFontFamily();
  }

  return winrt::Windows::UI::Xaml::Media::FontFamily(fontFamilyName);
}

} // namespace uwp
} // namespace react
