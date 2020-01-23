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
///   SetFont("MyFontName", 400, 0, "msappx://Assets/Fonts/MyFont.ttf#My Font");
///
///   auto fontFamily = GetFont("MyFontName", 400, 0); // Retrieves FontFamily pointing to MyFont.ttf
/// </example>
class FontManager {
 public:
  FontManager() = default;

  /// <summary>
  /// Configures an alias mapping [fontFamily, weight, style] to a FontFamily with the specified path.
  /// </summary>
  /// <param name="fontFamily"></param>
  /// <param name="weight"></param>
  /// <param name="style"></param>
  /// <param name="filePath">The path to the font file, suffixed with '#' and the name of
  /// the font family inside the file (e.g. "/Assets/Fonts/MyFont.ttf#My Font")</param>
  void SetFont(
      const std::wstring &fontFamily,
      winrt::Windows::UI::Text::FontWeight weight,
      winrt::Windows::UI::Text::FontStyle style,
      const std::wstring &filePath);

  /// <summary>
  /// Gets the closest matching FontFamily instance
  /// </summary>
  /// <param name="fontFamily"></param>
  /// <param name="weight"></param>
  /// <param name="style"></param>
  winrt::Windows::UI::Xaml::Media::FontFamily GetFont(
      const std::wstring &fontFamily,
      winrt::Windows::UI::Text::FontWeight weight,
      winrt::Windows::UI::Text::FontStyle style);

 private:
  std::unordered_map<FontKey, winrt::Windows::UI::Xaml::Media::FontFamily, FontKeyHasher> m_fonts;
};

} // namespace uwp
} // namespace react
