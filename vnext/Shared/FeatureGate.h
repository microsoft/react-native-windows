#pragma once

#include <string>

namespace Microsoft::React {

void __cdecl SetFeatureGate(std::string &&name) noexcept;

const bool __cdecl GetFeatureGate(std::string &&name) noexcept;

} // namespace Microsoft::React
