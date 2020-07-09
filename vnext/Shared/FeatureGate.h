#pragma once

#include <string>

namespace Microsoft::React {

void SetFeatureGate(std::string &&name) noexcept;

const bool GetFeatureGate(std::string &&name) noexcept;

} // namespace Microsoft::React
