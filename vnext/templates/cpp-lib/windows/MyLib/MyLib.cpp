#include "pch.h"

#include "{{ name }}.h"

namespace winrt::{{ namespaceCpp }}
{

// See https://microsoft.github.io/react-native-windows/docs/native-modules for details on writing native modules

void {{ name }}::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

double {{ name }}::multiply(double a, double b) noexcept {
  return a * b;
}

} // namespace winrt::{{ namespaceCpp }}