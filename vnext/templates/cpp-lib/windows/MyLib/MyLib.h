#pragma once

#include "pch.h"
#include "resource.h"

#if __has_include("codegen/Native{{ codegenName }}DataTypes.g.h")
  #include "codegen/Native{{ codegenName }}DataTypes.g.h"
#endif

// Note: The following lines use Mustache template syntax which will be processed during
// project generation to produce standard C++ code. The spec filename is derived from
// `codegenConfig.name` in the user library's package.json, which is the single source of truth.
#include "codegen/Native{{ codegenName }}Spec.g.h"

#include "NativeModules.h"

namespace winrt::{{ namespaceCpp }}
{

// See https://microsoft.github.io/react-native-windows/docs/native-platform for help writing native modules

REACT_MODULE({{ pascalName }})
struct {{ pascalName }}
{
  // Note: Mustache template syntax below will be processed during project generation
  // to produce standard C++ code based on the codegenConfig.name value.
  using ModuleSpec = {{ namespaceCpp }}Codegen::{{ codegenName }}Spec;

  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;

  REACT_SYNC_METHOD(multiply)
  double multiply(double a, double b) noexcept;

private:
  React::ReactContext m_context;
};

} // namespace winrt::{{ namespaceCpp }}
