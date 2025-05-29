#pragma once

#include "pch.h"
#include "resource.h"

#if __has_include("codegen/Native{{ pascalName }}DataTypes.g.h")
  #include "codegen/Native{{ pascalName }}DataTypes.g.h"
#endif
#include "codegen/Native{{ pascalName }}Spec.g.h"

#include "NativeModules.h"

namespace winrt::{{ namespaceCpp }}
{

// See https://microsoft.github.io/react-native-windows/docs/native-platform for help writing native modules

REACT_MODULE({{ pascalName }})
struct {{ pascalName }}
{
  using ModuleSpec = {{ namespaceCpp }}Codegen::{{ pascalName }}Spec;

  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;

  REACT_SYNC_METHOD(multiply)
  double multiply(double a, double b) noexcept;

private:
  React::ReactContext m_context;
};

} // namespace winrt::{{ namespaceCpp }}