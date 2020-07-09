#pragma once

#include <string>

namespace Microsoft::React {

/// <summary>
/// Sets a global boolean value identified by a random string.
/// </summary>
/// <param name="name">Global boolean key</param>
void __cdecl SetFeatureGate(std::string &&name, bool value) noexcept;

/// <summary>
/// Retrieves a global boolean value for the given key.
/// </summary>
/// <param name="name">Global boolean key</param>
/// <returns>Value stored for the given key, or false if the entry doesn't exist (default)</returns>
const bool __cdecl GetFeatureGate(std::string &&name) noexcept;

} // namespace Microsoft::React
