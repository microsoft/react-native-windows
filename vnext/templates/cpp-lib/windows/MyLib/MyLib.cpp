#include "pch.h"

#include "{{ name }}.h"

namespace winrt::{{ namespaceCpp }}
{

// See https://microsoft.github.io/react-native-windows/docs/native-platform for help writing native modules

void {{ pascalName }}::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

double {{ pascalName }}::multiply(double a, double b) noexcept {
  return a * b;
}

} // namespace winrt::{{ namespaceCpp }}