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

1. **Spec File Creation**: Checks for existing TurboModule spec files (Native*.ts/js). If none found, creates a default spec file with the proper TurboModule interface.

2. **Package.json Updates**: Adds or updates the `codegenConfig` section in package.json with proper Windows codegen configuration.

3. **Dependency Management**: 
   - Removes node_modules directory
   - Installs dependencies
   - Upgrades React Native and React Native Windows to latest versions (unless `--skip-deps` is used)
   - Reinstalls dependencies

4. **Windows Library Setup**: Runs `init-windows --template cpp-lib` to create the Windows-specific project structure.

5. **Code Generation**: Runs `codegen-windows` to generate C++ spec files from TypeScript/JavaScript specs.

6. **Stub File Generation**: Creates C++ header (.h) and implementation (.cpp) stub files with proper boilerplate code.

7. **Build Verification**: Checks if the project can be built (unless `--skip-build` is used).

## Generated Files

### Spec File (NativeModuleName.ts)
```typescript
import type {TurboModule} from 'react-native/Libraries/TurboModule/RCTExport';
import {TurboModuleRegistry} from 'react-native';

export interface Spec extends TurboModule {
  // Add your module methods here
  // Example:
  // getString(value: string): Promise<string>;
}

export default TurboModuleRegistry.getEnforcing<Spec>('ModuleName');
```

### C++ Header (ModuleName.h)
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
  
  // TODO: Add your method implementations here
};

} // namespace ModuleNameSpecs
```

### C++ Implementation (ModuleName.cpp)
```cpp
#include "ModuleName.h"

namespace ModuleNameSpecs {

void ModuleName::Initialize(React::ReactContext const &reactContext) noexcept {
  // TODO: Initialize your module
}

// TODO: Implement your methods here

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

1. **Update the spec file** (`NativeModuleName.ts`) with your module's actual interface
2. **Implement the methods** in the generated C++ stub files
3. **Build your project** to verify everything works correctly

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