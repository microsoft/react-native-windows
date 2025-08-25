# module-windows-setup Command

The `module-windows-setup` command streamlines the process of adding Windows support to React Native community modules. It automates the complete workflow from spec file creation to C++ stub generation.

## Usage

```bash
yarn react-native module-windows-setup [options]
```

## Options

- `--logging`: Enable verbose output logging
- `--no-telemetry`: Disable telemetry tracking
- `--skip-deps`: Skip dependency upgrades (use current versions)
- `--skip-build`: Skip final build verification step

## What it does

The command performs the following steps automatically:

1. **Intelligent API Discovery**: Analyzes existing APIs from multiple sources:
   - Existing JavaScript/TypeScript files (main entry point, index files)
   - Android native implementation (`@ReactMethod` annotations in Java/Kotlin files)
   - iOS native implementation (`RCT_EXPORT_METHOD` in Objective-C files)
   - README documentation (method signatures in code blocks)

2. **Smart Spec File Creation**: Creates TurboModule spec files with actual method signatures discovered from analysis, or generates a template if no APIs are found.

3. **Package.json Updates**: Adds or updates the `codegenConfig` section in package.json with proper Windows codegen configuration.

4. **Dependency Management**: 
   - Removes node_modules directory
   - Installs dependencies
   - Upgrades React Native and React Native Windows to latest versions (unless `--skip-deps` is used)
   - Reinstalls dependencies

5. **Windows Library Setup**: Runs `init-windows --template cpp-lib` to create the Windows-specific project structure.

6. **Code Generation**: Runs `codegen-windows` to generate C++ spec files from TypeScript/JavaScript specs.

7. **Intelligent C++ Stub Generation**: Creates C++ header (.h) and implementation (.cpp) stub files with:
   - Proper method signatures matching the TypeScript spec
   - Correct parameter types mapped from TypeScript to C++
   - Promise-based async methods or synchronous methods as appropriate
   - Example implementations and proper return value handling

8. **Build Verification**: Checks if the project can be built (unless `--skip-build` is used).

## Generated Files

### Spec File (NativeModuleName.ts)
Generated based on API analysis from existing implementations:

```typescript
import type {TurboModule} from 'react-native/Libraries/TurboModule/RCTExport';
import {TurboModuleRegistry} from 'react-native';

export interface Spec extends TurboModule {
  // Methods discovered from existing Android/iOS/JS implementations
  getString(value: string): Promise<string>;
  getNumber(count: number): Promise<number>;
  getBoolValue(): Promise<boolean>;
  performAction(config: object): Promise<void>;
}

export default TurboModuleRegistry.getEnforcing<Spec>('ModuleName');
```

### C++ Header (ModuleName.h)
Generated with actual method signatures matching the spec:

```cpp
#pragma once

#include <ModuleNameSpec.g.h>
#include <NativeModules.h>

namespace ModuleNameSpecs {

REACT_MODULE(ModuleName)
struct ModuleName {
  using ModuleSpec = ModuleNameSpec;
  
  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;
  
  REACT_METHOD(getString)
  void getString(std::string value, React::ReactPromise<std::string> promise) noexcept;
  
  REACT_METHOD(getNumber)
  void getNumber(double count, React::ReactPromise<double> promise) noexcept;
  
  REACT_METHOD(getBoolValue)
  void getBoolValue(React::ReactPromise<bool> promise) noexcept;
  
  REACT_METHOD(performAction)
  void performAction(React::JSValue config, React::ReactPromise<React::JSValue> promise) noexcept;
};

} // namespace ModuleNameSpecs
```

### C++ Implementation (ModuleName.cpp)
Generated with method stubs that match the spec:

```cpp
#include "ModuleName.h"

namespace ModuleNameSpecs {

void ModuleName::Initialize(React::ReactContext const &reactContext) noexcept {
  // TODO: Initialize your module
}

void ModuleName::getString(std::string value, React::ReactPromise<std::string> promise) noexcept {
  // TODO: Implement getString
  promise.Resolve("example");
}

void ModuleName::getNumber(double count, React::ReactPromise<double> promise) noexcept {
  // TODO: Implement getNumber
  promise.Resolve(42);
}

void ModuleName::getBoolValue(React::ReactPromise<bool> promise) noexcept {
  // TODO: Implement getBoolValue
  promise.Resolve(true);
}

void ModuleName::performAction(React::JSValue config, React::ReactPromise<React::JSValue> promise) noexcept {
  // TODO: Implement performAction
  promise.Resolve();
}

} // namespace ModuleNameSpecs
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

## Next Steps

After running the command:

1. **Review the generated spec file** (`NativeModuleName.ts`) - The command analyzes existing Android/iOS implementations and README documentation to discover APIs, but you should verify the generated method signatures match your intended interface
2. **Implement the methods** in the generated C++ stub files - The stubs are generated with proper signatures matching the spec file
3. **Build your project** to verify everything works correctly
4. **Add any missing methods** that weren't discovered during analysis

## API Discovery Process

The command intelligently discovers APIs from multiple sources:

### 1. JavaScript/TypeScript Analysis
- Main entry point specified in package.json
- Common file locations (index.js, index.ts, src/index.js, src/index.ts)
- Exported functions and method definitions

### 2. Android Implementation Analysis
- Scans `android/` directory for Java/Kotlin files
- Looks for `@ReactMethod` annotations
- Extracts method names and basic type information

### 3. iOS Implementation Analysis  
- Scans `ios/` directory for Objective-C files
- Looks for `RCT_EXPORT_METHOD` declarations
- Extracts method names

### 4. Documentation Analysis
- Parses README.md for method signatures in code blocks
- Extracts function-like patterns

The discovered methods are combined and deduplicated to create a comprehensive API specification.

## Examples

Basic usage:
```bash
yarn react-native module-windows-setup
```

With verbose logging:
```bash
yarn react-native module-windows-setup --logging
```

Skip dependency upgrades:
```bash
yarn react-native module-windows-setup --skip-deps
```

Skip build verification:
```bash
yarn react-native module-windows-setup --skip-build
```