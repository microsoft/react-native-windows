#pragma once

#include <string>

namespace Microsoft::React {

void SetFeatureGate(std::string &&name) noexcept;

bool GetFeatureGate(std::string &&name) noexcept;

}
