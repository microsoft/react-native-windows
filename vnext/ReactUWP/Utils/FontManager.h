// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <stdint.h>
#include <string>
#include <unordered_map>

namespace react {
namespace uwp {

// Helper for generating a hash from several values
template <typename T, typename... Rest>
inline void hash_combine(std::size_t &seed, const T &v, const Rest &... rest) {
  seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  (hash_combine(seed, rest), ...);
}

struct FontKey {
  std::wstring familyName;
  std::int32_t weight;
  std::int32_t style;

  bool operator==(const FontKey &other) const {
    return (familyName == other.familyName && weight == other.weight && style == other.style);
  }
};

struct FontKeyHasher {
  std::size_t operator()(const FontKey &k) const {
    std::size_t seed = 0;
    hash_combine(seed, k.familyName, k.weight, k.style);
    return seed;
  }
};

/// <summary>
/// Manages loading, caching, and aliasing of FontFamily objects. Permits registering locally bundled
/// font files with short names to simplify usage in React.
/// </summary>
/// <example>
///   SetFont("MyFontName", 400, 0, "msappx:///Assets/Fonts/MyFont.ttf#My Font");
///
///   auto fontFamily = GetFont("MyFontName", 400, 0); // Retrieves FontFamily pointing to MyFont.ttf
/// </example>
class FontManager {
 public:
  static FontManager getInstance();

  /// <summary>
  /// Configures an alias mapping [fontFamilyName, weight, style] to a FontFamily with the specified path.
  /// </summary>
  /// <param name="fontFamilyName">The name of the font family</param>
  /// <param name="weight">The weight of the font</param>
  /// <param name="style">The style of the font, such as italic</param>
  /// <param name="filePath">Either a path-like string in the form of "ms-appx:///Assets/Fonts/FontFile.ttf#Font Name" or a system font name.</param>
  void SetFont(
      const std::wstring &fontFamilyName,
      winrt::Windows::UI::Text::FontWeight weight,
      winrt::Windows::UI::Text::FontStyle style,
      const std::wstring &filePath);

  /// <summary>
  /// Gets the closest matching FontFamily instance. In order, it searches for:
  /// 1) Exact match in alias map.
  /// 2) [fontFamilyName, Normal, Normal] in alias map
  /// 3) The system font with the given name
  /// </summary>
  /// <param name="fontFamilyName"></param>
  /// <param name="weight"></param>
  /// <param name="style"></param>
  winrt::Windows::UI::Xaml::Media::FontFamily GetFont(
      const std::wstring &fontFamilyName,
      winrt::Windows::UI::Text::FontWeight weight,
      winrt::Windows::UI::Text::FontStyle style);

 private:
  FontManager() = default;

  std::unordered_map<FontKey, winrt::Windows::UI::Xaml::Media::FontFamily, FontKeyHasher> m_fonts;
};

} // namespace uwp
} // namespace react
