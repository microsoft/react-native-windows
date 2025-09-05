# Complete Example: Setting up Windows support for react-native-webview

This example demonstrates how to use the `setup-module-windows` command to add Windows support to the popular `react-native-webview` community module.

## Prerequisites

You must have a TurboModule spec file before running the setup command. The command will fail if no spec file exists.

## Step 1: Create TurboModule Spec File

First, create the spec file (`NativeReactNativeWebview.ts`):

```typescript
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import type {TurboModule} from 'react-native/Libraries/TurboModule/RCTExport';
import {TurboModuleRegistry} from 'react-native';

export interface Spec extends TurboModule {
  goBack(): void;
  goForward(): void;
  reload(): void;
  stopLoading(): void;
  loadUrl(url: string): void;
  evaluateJavaScript(script: string): Promise<string>;
}

export default TurboModuleRegistry.getEnforcing<Spec>('ReactNativeWebview');
```

## Step 2: Run the Setup Command

### For Library Module (default)
```bash
cd react-native-webview
yarn react-native setup-module-windows --logging
```

### For App Module
```bash
cd react-native-webview
yarn react-native setup-module-windows --template cpp-app --logging
```

## Step 3: Command Output

```
✔ Setting up Windows support for React Native module...
[SetupModuleWindows] Validating environment...
[SetupModuleWindows] Project name: react-native-webview
[SetupModuleWindows] Yarn found
[SetupModuleWindows] Checking for TurboModule spec file...
[SetupModuleWindows] Found valid spec file(s): NativeReactNativeWebview.ts
[SetupModuleWindows] Extracted actual module name: ReactNativeWebview
[SetupModuleWindows] Added codegenConfig to package.json with module name: ReactNativeWebview
[SetupModuleWindows] Installing dependencies...
[SetupModuleWindows] Dependencies installed
[SetupModuleWindows] Running init-windows with cpp-lib template...
[SetupModuleWindows] init-windows completed successfully
[SetupModuleWindows] Running codegen-windows...
[SetupModuleWindows] codegen-windows completed successfully
[SetupModuleWindows] Generating C++ stub files...
[SetupModuleWindows] Generated header stub: /path/to/windows/ReactNativeWebview/ReactNativeWebview.h
[SetupModuleWindows] Generated cpp stub: /path/to/windows/ReactNativeWebview/ReactNativeWebview.cpp

🎉 Your React Native module now supports Windows!

Files created/updated:
📄 package.json - Added codegen configuration
💻 windows/ReactNativeWebview/ReactNativeWebview.h - C++ header file (implement your methods here)
⚙️  windows/ReactNativeWebview/ReactNativeWebview.cpp - C++ implementation file (add your logic here)

Next steps:
1. 📝 Update the generated spec file with your module's interface
2. 🔧 Implement the methods in the generated C++ stub files
3. 🏗️  Build your project to verify everything works
4. 📚 See the documentation for more details on TurboModule development

For help, visit: https://microsoft.github.io/react-native-windows/
```

## Step 4: Final Project Structure

After running the command, your project will have this structure:

### For cpp-lib template (default):
```
react-native-webview/
├── package.json (updated with codegenConfig)
├── NativeReactNativeWebview.ts (your spec file)
├── windows/
│   ├── ReactNativeWebview/
│   │   ├── ReactNativeWebview.h (generated)
│   │   └── ReactNativeWebview.cpp (generated)
│   └── ReactNativeWebview.sln (from init-windows)
├── codegen/
│   └── ReactNativeWebviewSpec.g.h (from codegen-windows)
└── README.md
```

### For cpp-app template:
```
react-native-webview/
├── package.json (updated with codegenConfig)
├── NativeReactNativeWebview.ts (your spec file)
├── windows/
│   ├── App.xaml
│   ├── MainWindow.xaml
│   ├── ReactNativeWebview/
│   │   ├── ReactNativeWebview.h (generated)
│   │   └── ReactNativeWebview.cpp (generated)
│   └── ReactNativeWebview.sln (from init-windows)
├── codegen/
│   └── ReactNativeWebviewSpec.g.h (from codegen-windows)
└── README.md
```

## Step 5: Generated Files Content

### Header File (ReactNativeWebview.h)

```cpp
#pragma once

#include "pch.h"
#include "resource.h"

#include "codegen/NativeReactNativeWebviewSpec.g.h"
#include "NativeModules.h"

namespace winrt::ReactNativeWebviewSpecs
{

REACT_MODULE(ReactNativeWebview)
struct ReactNativeWebview
{
  using ModuleSpec = ReactNativeWebviewCodegen::ReactNativeWebviewSpec;

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

} // namespace winrt::ReactNativeWebviewSpecs
```

### Implementation File (ReactNativeWebview.cpp)

```cpp
#include "ReactNativeWebview.h"

namespace winrt::ReactNativeWebviewSpecs {

void ReactNativeWebview::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

void ReactNativeWebview::sayHello(std::string name, std::function<void(std::string)> const & callback) noexcept {
  std::string result = "Hello " + name + "! Module is working.";
  callback(result);
}

// TODO: Implement your methods here

} // namespace winrt::ReactNativeWebviewSpecs
```

## Step 6: Build and Test

```bash
# Build the Windows project
cd windows
msbuild ReactNativeWebview.sln

# Or use Visual Studio to open and build the solution
```

## Template Differences

### cpp-lib Template (Default)
- **Use case**: Native modules that provide functionality to React Native apps
- **Structure**: Library project with module files in subdirectory
- **Best for**: Community modules, utility libraries, platform bridges

### cpp-app Template
- **Use case**: Standalone Windows applications with React Native UI
- **Structure**: Full application project with XAML UI files
- **Best for**: Complete Windows apps, enterprise applications

## Command Options

### Template Selection
```bash
# Use cpp-lib template (default)
yarn react-native setup-module-windows

# Use cpp-app template
yarn react-native setup-module-windows --template cpp-app

# Use cpp-lib template explicitly
yarn react-native setup-module-windows --template cpp-lib
```

### Other Options
```bash
# Verbose logging
yarn react-native setup-module-windows --logging

# Skip dependency upgrades
yarn react-native setup-module-windows --skip-deps

# Skip build verification
yarn react-native setup-module-windows --skip-build

# Combine options
yarn react-native setup-module-windows --template cpp-app --logging --skip-deps
```

## Error Handling

If you get the error: **"Create Spec File - TurboModule spec file not found"**

This means you need to create a TurboModule spec file first. The command will not automatically create spec files - you must create them manually before running the setup.

## Benefits of Using setup-module-windows

1. **Template Flexibility**: Choose between library and application project structures
2. **Validation**: Ensures TurboModule spec exists before proceeding
3. **Best Practices**: Generated files follow RNW coding standards and patterns
4. **Completeness**: Creates the entire Windows integration structure in one command
5. **Consistency**: Ensures consistent naming and structure across all Windows modules
6. **Reference Code**: Includes multiply function reference and working Hello World example