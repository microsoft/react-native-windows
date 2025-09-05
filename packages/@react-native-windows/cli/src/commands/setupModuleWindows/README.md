# setup-module-windows Command

The `setup-module-windows` command streamlines the process of adding Windows support to React Native community modules. It automates the complete workflow from spec file creation to C++ stub generation with working Hello World examples.

## Usage

```bash
yarn react-native setup-module-windows [options]
```

## Options

- `--logging`: Enable verbose output logging
- `--no-telemetry`: Disable telemetry tracking
- `--skip-deps`: Skip dependency upgrades (use current versions)
- `--skip-build`: Skip final build verification step

## What it does

The command performs the following 10 steps automatically as defined in [issue #15078](https://github.com/microsoft/react-native-windows/issues/15078):

1. **TurboModule Spec Detection/Creation**: Checks for existing TurboModule spec files. If none found, creates a spec file with Hello World methods instead of erroring out.

2. **Package.json Updates**: Adds or updates the `codegenConfig` section in package.json with proper Windows codegen configuration.

3. **Dependency Cleanup**: Removes node_modules directory for a clean start.

4. **Dependency Installation**: Runs `yarn install` to install dependencies.

5. **Dependency Upgrades**: Upgrades React Native and React Native Windows to latest versions (unless `--skip-deps` is used).

6. **Dependency Reinstallation**: Runs `yarn install` again after upgrades.

7. **Windows Library Setup**: Runs `init-windows --template cpp-lib` to create the Windows-specific project structure.

8. **Code Generation**: Runs `codegen-windows` to generate C++ spec files from TypeScript/JavaScript specs.

9. **C++ Stub Generation**: Creates C++ header (.h) and implementation (.cpp) stub files with Hello World reference methods.

10. **Build Verification**: Checks if the project can be built (unless `--skip-build` is used).

## Generated Files

### TurboModule Spec File (NativeModuleName.ts)
Generated with Hello World methods for immediate testing:

```typescript
export interface Spec extends TurboModule {
  // Hello World method - A simple greeting function to verify the module works
  sayHello(name: string): Promise<string>;
  
  // Reference multiply function - Keep this as a learning example
  // This demonstrates how to handle numeric operations in a TurboModule
  multiply(a: number, b: number): Promise<number>;
  
  // Example async method with no parameters
  getRandomNumber(): Promise<number>;
  
  // Example method with boolean return
  isModuleReady(): Promise<boolean>;
}
```

### C++ Header (ModuleName.h)
Generated with proper method signatures matching the spec:

```cpp
REACT_MODULE(ModuleName)
struct ModuleName {
  using ModuleSpec = ModuleNameCodegen::ModuleNameSpec;
  
  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;
  
  REACT_METHOD(sayHello)
  void sayHello(std::string name, React::ReactPromise<std::string> promise) noexcept;
  
  REACT_METHOD(multiply)
  void multiply(double a, double b, React::ReactPromise<double> promise) noexcept;
  
  // ... other methods
};
```

### C++ Implementation (ModuleName.cpp)
Generated with working Hello World implementations:

```cpp
void ModuleName::sayHello(std::string name, React::ReactPromise<std::string> promise) noexcept {
  // Hello World implementation - demonstrates string handling
  try {
    std::string greeting = "Hello, " + name + "! Welcome to React Native Windows.";
    promise.Resolve(greeting);
  } catch (const std::exception& e) {
    promise.Reject(React::ReactError{e.what()});
  }
}

void ModuleName::multiply(double a, double b, React::ReactPromise<double> promise) noexcept {
  // Reference multiply function - demonstrates numeric operations
  // Keep this as an example for new module developers
  try {
    double result = a * b;
    promise.Resolve(result);
  } catch (const std::exception& e) {
    promise.Reject(React::ReactError{e.what()});
  }
}
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
      "outputDirectory": "windows/ModuleName/codegen",
      "separateDataTypes": true
    }
  }
}
```

## Hello World Methods

The generated spec includes these methods ready for immediate testing:

- **`sayHello(name: string)`** - Demonstrates string handling and basic promise resolution
- **`multiply(a: number, b: number)`** - Reference implementation kept for learning purposes
- **`getRandomNumber()`** - Example async method with no parameters
- **`isModuleReady()`** - Example boolean return method

## Next Steps

After running the command:

1. **Test the Hello World methods** - The generated methods are fully functional
2. **Add your module's specific API** - Update the spec file with your required methods
3. **Implement your business logic** - Replace Hello World implementations with real functionality
4. **Build your project** to verify everything works correctly

## Examples

Basic usage:
```bash
yarn react-native setup-module-windows
```

With verbose logging:
```bash
yarn react-native setup-module-windows --logging
```

Skip dependency upgrades (faster for testing):
```bash
yarn react-native setup-module-windows --skip-deps
```

Skip build verification:
```bash
yarn react-native setup-module-windows --skip-build
```

## Success Output

When successful, the command outputs:

```
🎉 Your React Native module now supports Windows!

Files created/updated:
📄 package.json - Added codegen configuration
🏗️  NativeModuleName.ts - TurboModule spec file with Hello World methods
💻 windows/ModuleName/ModuleName.h - C++ header file with Hello World stubs
⚙️  windows/ModuleName/ModuleName.cpp - C++ implementation file with working examples

Hello World methods included:
• sayHello(name) - Demonstrates string handling
• multiply(a, b) - Reference implementation for learning
• getRandomNumber() - Example async method
• isModuleReady() - Example boolean return method

Next steps:
1. 📝 Test the Hello World methods or add your module's API
2. 🔧 Implement your specific business logic
3. 🏗️  Build your project to verify everything works
4. 📚 See the documentation for more details on TurboModule development
```

## Benefits

1. **Automation**: All setup steps are automated - no manual file creation or configuration
2. **Best Practices**: Generated files follow RNW coding standards and patterns
3. **Completeness**: Creates the entire Windows integration structure in one command
4. **Working Examples**: Includes functional Hello World methods that work immediately
5. **Learning Support**: Keeps reference multiply function with educational comments
6. **Error Prevention**: Validates environment and handles common setup issues
7. **Clear Guidance**: Provides step-by-step next actions

This command transforms any React Native community module into a fully Windows-compatible module with minimal effort and immediate working examples.