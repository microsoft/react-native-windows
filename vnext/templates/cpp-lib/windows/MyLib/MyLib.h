#pragma once

#include "pch.h"
#include "resource.h"

#if __has_include("codegen/Native{{ pascalName }}DataTypes.g.h")
  #include "codegen/Native{{ pascalName }}DataTypes.g.h"
#endif
// Note: The following lines use Mustache template syntax which will be processed during
// project generation to produce standard C++ code. If existing codegen spec files are found,
// use the actual filename; otherwise use conditional includes.
{{#hasExistingSpecFiles}}
#include "codegen/{{ firstSpecFile }}"
{{/hasExistingSpecFiles}}
{{^hasExistingSpecFiles}}
#if __has_include("codegen/Native{{ pascalName }}Spec.g.h")
  #include "codegen/Native{{ pascalName }}Spec.g.h"
#endif
{{/hasExistingSpecFiles}}

#include "NativeModules.h"

namespace winrt::{{ namespaceCpp }}
{

// See https://microsoft.github.io/react-native-windows/docs/native-platform for help writing native modules

REACT_MODULE({{ pascalName }})
struct {{ pascalName }}
{
  // Note: Mustache template syntax below will be processed during project generation
  // to produce standard C++ code based on detected codegen files.
{{#hasExistingSpecFiles}}
  using ModuleSpec = {{ namespaceCpp }}Codegen::{{ firstSpecName }};
{{/hasExistingSpecFiles}}
{{^hasExistingSpecFiles}}
#if __has_include("codegen/Native{{ pascalName }}Spec.g.h")
  using ModuleSpec = {{ namespaceCpp }}Codegen::{{ pascalName }}Spec;
#endif
{{/hasExistingSpecFiles}}

  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;

  REACT_SYNC_METHOD(multiply)
  double multiply(double a, double b) noexcept;

private:
  React::ReactContext m_context;
};

} // namespace winrt::{{ namespaceCpp }}