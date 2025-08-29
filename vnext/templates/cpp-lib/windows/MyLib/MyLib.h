#pragma once

#include "pch.h"
#include "resource.h"

#if __has_include("codegen/Native{{ pascalName }}DataTypes.g.h")
  #include "codegen/Native{{ pascalName }}DataTypes.g.h"
#endif
// TODO: Update this include to match your actual TurboModule name
// For example, if your module is named 'MyCustomModule', use:
// #include "codegen/NativeMyCustomModuleSpec.g.h"
#if __has_include("codegen/Native{{ pascalName }}Spec.g.h")
  #include "codegen/Native{{ pascalName }}Spec.g.h"
#endif

#include "NativeModules.h"

namespace winrt::{{ namespaceCpp }}
{

// See https://microsoft.github.io/react-native-windows/docs/native-platform for help writing native modules

REACT_MODULE({{ pascalName }})
struct {{ pascalName }}
{
  // TODO: Update this to match your actual TurboModule spec name
  // For example, if your module is named 'MyCustomModule', use:
  // using ModuleSpec = {{ namespaceCpp }}Codegen::MyCustomModuleSpec;
#ifdef __has_include
#if __has_include("codegen/Native{{ pascalName }}Spec.g.h")
  using ModuleSpec = {{ namespaceCpp }}Codegen::{{ pascalName }}Spec;
#endif
#else
  using ModuleSpec = {{ namespaceCpp }}Codegen::{{ pascalName }}Spec;
#endif

  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;

  REACT_SYNC_METHOD(multiply)
  double multiply(double a, double b) noexcept;

private:
  React::ReactContext m_context;
};

} // namespace winrt::{{ namespaceCpp }}