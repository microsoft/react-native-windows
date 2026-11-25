#pragma once

#include "pch.h"
#include "resource.h"

#if __has_include("codegen/{{ codegenDataTypesFile }}")
  #include "codegen/{{ codegenDataTypesFile }}"
#endif

#if __has_include("codegen/{{ codegenSpecFile }}")
  #include "codegen/{{ codegenSpecFile }}"
#endif

#include "NativeModules.h"

namespace winrt::{{ namespaceCpp }}
{

// See https://microsoft.github.io/react-native-windows/docs/native-platform for help writing native modules

REACT_MODULE({{ pascalName }})
struct {{ pascalName }}
{
#if __has_include("codegen/{{ codegenSpecFile }}")
  using ModuleSpec = {{ namespaceCpp }}Codegen::{{ codegenProjectName }};
#endif

  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;

  REACT_SYNC_METHOD(multiply)
  double multiply(double a, double b) noexcept;

private:
  React::ReactContext m_context;
};

} // namespace winrt::{{ namespaceCpp }}
