# setup-module-windows Command

The `setup-module-windows` command streamlines the process of adding Windows support to React Native community modules. It requires an existing TurboModule spec file and automates the Windows project setup.

## Usage

```bash
yarn react-native setup-module-windows [options]
```

## Options

- `--logging`: Enable verbose output logging
- `--no-telemetry`: Disable telemetry tracking
- `--skip-deps`: Skip dependency upgrades (use current versions)
- `--skip-build`: Skip final build verification step
- `--template <template>`: Project template (cpp-lib or cpp-app), defaults to cpp-lib

## What it does

The command performs the following steps automatically:

1. **TurboModule Spec Validation**: Checks for existing TurboModule spec files and fails if none are found. You must create a TurboModule spec file before running this command.

2. **Package.json Updates**: Adds or updates the `codegenConfig` section in package.json with proper Windows codegen configuration.

3. **Dependency Management**: 
   - Installs dependencies using yarn

4. **Windows Library Setup**: Runs `init-windows --template <template>` to create the Windows-specific project structure. Supports both `cpp-lib` (default) and `cpp-app` templates.

5. **Code Generation**: Runs `codegen-windows` to generate C++ spec files from TypeScript/JavaScript specs.

6. **C++ Stub Generation**: Creates C++ header (.h) and implementation (.cpp) stub files with:
   - Proper method signatures matching the TypeScript spec
   - Reference multiply function as commented example (from cpp-lib template)
   - Hello World `sayHello` function to verify module functionality
   - TODO comments for implementing additional methods

## Generated Files

### C++ Header (ModuleName.h)
Generated with proper module structure:

```cpp
#pragma once

#include "pch.h"
#include "resource.h"

#include "codegen/NativeModuleNameSpec.g.h"
#include "NativeModules.h"

namespace winrt::ModuleNameSpecs
{

REACT_MODULE(ModuleName)
struct ModuleName
{
  using ModuleSpec = ModuleNameCodegen::ModuleNameSpec;

  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;

  // Reference function for demonstration (from cpp-lib template)
  // double multiply(double a, double b) noexcept { return a * b; }

  // Hello World example to verify module functionality
  REACT_METHOD(sayHello)
  void sayHello(std::string name, std::function<void(std::string)> const & callback) noexcept;

  // TODO: Add your method implementations here

private:
  React::ReactContext m_context;
};

} // namespace winrt::ModuleNameSpecs
```

### C++ Implementation (ModuleName.cpp)
Generated with method stubs including working Hello World example:

```cpp
#include "ModuleName.h"

namespace winrt::ModuleNameSpecs {

void ModuleName::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

void ModuleName::sayHello(std::string name, std::function<void(std::string)> const & callback) noexcept {
  std::string result = "Hello " + name + "! Module is working.";
  callback(result);
}

// TODO: Implement your methods here

} // namespace winrt::ModuleNameSpecs
```

## Package.json Configuration

The command adds this configuration to your package.json:

```json
{
  "codegenConfig": {
    "name": "ModuleName",
    "type": "modules",
    "jsSrcsDir": ".",
    "windows": {
      "namespace": "ModuleNameSpecs",
      "outputDirectory": "codegen",
      "generators": ["modulesWindows"]
    }
  }
}
```

## Prerequisites

1. **TurboModule Spec File**: You must create a TurboModule spec file before running this command. The command will fail with an error if no spec file is found.

Example spec file (NativeModuleName.ts):
```typescript
import type {TurboModule} from 'react-native/Libraries/TurboModule/RCTExport';
import {TurboModuleRegistry} from 'react-native';

export interface Spec extends TurboModule {
  getString(value: string): Promise<string>;
  getNumber(value: number): Promise<number>;
}

export default TurboModuleRegistry.getEnforcing<Spec>('ModuleName');
```

## Template Options

### cpp-lib (Default)
- Creates a library project structure
- Best for native modules that provide functionality to other apps
- Generates files in `windows/ModuleName/` directory

### cpp-app
- Creates an application project structure  
- Best for standalone applications with Windows-specific UI
- Generates files in `windows/` directory with app structure

## Next Steps

After running the command:

1. **Implement the methods** in the generated C++ stub files - The Hello World function is already implemented as an example
2. **Build your project** to verify everything works correctly
3. **Add any additional methods** to your TurboModule spec and C++ implementation

## Examples

Basic usage with default cpp-lib template:
```bash
yarn react-native setup-module-windows
```

With cpp-app template:
```bash
yarn react-native setup-module-windows --template cpp-app
```

With verbose logging:
```bash
yarn react-native setup-module-windows --logging
```

Skip dependency upgrades:
```bash
yarn react-native setup-module-windows --skip-deps
```

Skip build verification:
```bash
yarn react-native setup-module-windows --skip-build
```

## Error Messages

**"Create Spec File - TurboModule spec file not found"**: You need to create a TurboModule spec file before running this command. The command will not automatically create spec files.