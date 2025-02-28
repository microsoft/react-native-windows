#include "pch.h"

#include "ReactNativeXamlFabric.h"

namespace winrt::ReactNativeXamlFabric
{

// See https://microsoft.github.io/react-native-windows/docs/native-modules for details on writing native modules

void ReactNativeXamlFabric::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

double ReactNativeXamlFabric::multiply(double a, double b) noexcept {
  return a * b;
}

} // namespace winrt::ReactNativeXamlFabric