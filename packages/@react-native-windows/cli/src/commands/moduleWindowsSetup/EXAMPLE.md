# Complete Example: Setting up Windows support for react-native-webview

This example demonstrates how to use the `module-windows-setup` command to add Windows support to the popular `react-native-webview` community module.

## Starting Point

You have cloned or created a React Native module project with this structure:
```
react-native-webview/
├── package.json
├── index.js
├── src/
│   └── WebViewNativeComponent.ts
└── README.md
```

## Step 1: Run the Setup Command

```bash
cd react-native-webview
yarn react-native module-windows-setup --logging
```

## Step 2: Command Output

```
✔ Setting up Windows support for React Native module...
[ModuleWindowsSetup] Validating environment...
[ModuleWindowsSetup] Project name: react-native-webview
[ModuleWindowsSetup] Yarn found
[ModuleWindowsSetup] Checking for TurboModule spec file...
[ModuleWindowsSetup] No spec file found, creating default TurboModule spec...
[ModuleWindowsSetup] Created spec file: /path/to/NativeReactNativeWebview.ts
[ModuleWindowsSetup] Checking and updating package.json codegen configuration...
[ModuleWindowsSetup] Added codegenConfig to package.json
[ModuleWindowsSetup] Cleaning node_modules and reinstalling dependencies...
[ModuleWindowsSetup] Removed node_modules
[ModuleWindowsSetup] Dependencies installed
[ModuleWindowsSetup] Upgrading React Native and React Native Windows to latest versions...
[ModuleWindowsSetup] Latest RN version: 0.73.0
[ModuleWindowsSetup] Latest RNW version: 0.73.0
[ModuleWindowsSetup] Updated dependency versions in package.json
[ModuleWindowsSetup] Running init-windows with cpp-lib template...
[ModuleWindowsSetup] init-windows completed successfully
[ModuleWindowsSetup] Running codegen-windows...
[ModuleWindowsSetup] codegen-windows completed successfully
[ModuleWindowsSetup] Generating C++ stub files...
[ModuleWindowsSetup] Generated header stub: /path/to/windows/ReactNativeWebview.h
[ModuleWindowsSetup] Generated cpp stub: /path/to/windows/ReactNativeWebview.cpp
[ModuleWindowsSetup] Verifying build setup...
[ModuleWindowsSetup] MSBuild found, project should be buildable

🎉 Your React Native module now supports Windows!

Files created/updated:
📄 package.json - Added codegen configuration
🏗️  NativeReactNativeWebview.ts - TurboModule spec file (edit with your API)
💻 windows/ReactNativeWebview.h - C++ header file (implement your methods here)
⚙️  windows/ReactNativeWebview.cpp - C++ implementation file (add your logic here)

Next steps:
1. 📝 Update the generated spec file with your module's interface
2. 🔧 Implement the methods in the generated C++ stub files
3. 🏗️  Build your project to verify everything works
4. 📚 See the documentation for more details on TurboModule development

For help, visit: https://microsoft.github.io/react-native-windows/
```

## Step 3: Final Project Structure

After running the command, your project will have this structure:

```
react-native-webview/
├── package.json (updated with codegenConfig)
├── index.js
├── src/
│   └── WebViewNativeComponent.ts
├── NativeReactNativeWebview.ts (generated)
├── windows/
│   ├── ReactNativeWebview.h (generated)
│   ├── ReactNativeWebview.cpp (generated)
│   └── ReactNativeWebview.sln (from init-windows)
├── codegen/
│   └── ReactNativeWebviewSpec.g.h (from codegen-windows)
└── README.md
```

## Step 4: Customize the Generated Files

### Update the Spec File (NativeReactNativeWebview.ts)

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

### Update the Header File (windows/ReactNativeWebview.h)

```cpp
#pragma once

#include <ReactNativeWebviewSpec.g.h>
#include <NativeModules.h>

namespace ReactNativeWebviewSpecs {

REACT_MODULE(ReactNativeWebview)
struct ReactNativeWebview {
  using ModuleSpec = ReactNativeWebviewSpec;
  
  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;
  
  REACT_METHOD(goBack)
  void goBack() noexcept;
  
  REACT_METHOD(goForward)
  void goForward() noexcept;
  
  REACT_METHOD(reload)
  void reload() noexcept;
  
  REACT_METHOD(stopLoading)
  void stopLoading() noexcept;
  
  REACT_METHOD(loadUrl)
  void loadUrl(std::string url) noexcept;
  
  REACT_METHOD(evaluateJavaScript)
  void evaluateJavaScript(std::string script, React::ReactPromise<std::string> promise) noexcept;

private:
  React::ReactContext m_reactContext{nullptr};
};

} // namespace ReactNativeWebviewSpecs
```

### Update the Implementation File (windows/ReactNativeWebview.cpp)

```cpp
#include "ReactNativeWebview.h"

namespace ReactNativeWebviewSpecs {

void ReactNativeWebview::Initialize(React::ReactContext const &reactContext) noexcept {
  m_reactContext = reactContext;
}

void ReactNativeWebview::goBack() noexcept {
  // TODO: Implement WebView go back functionality
}

void ReactNativeWebview::goForward() noexcept {
  // TODO: Implement WebView go forward functionality
}

void ReactNativeWebview::reload() noexcept {
  // TODO: Implement WebView reload functionality
}

void ReactNativeWebview::stopLoading() noexcept {
  // TODO: Implement WebView stop loading functionality
}

void ReactNativeWebview::loadUrl(std::string url) noexcept {
  // TODO: Implement WebView load URL functionality
}

void ReactNativeWebview::evaluateJavaScript(std::string script, React::ReactPromise<std::string> promise) noexcept {
  try {
    // TODO: Implement JavaScript evaluation
    promise.Resolve("JavaScript evaluation result");
  } catch (const std::exception& e) {
    promise.Reject(React::ReactError{e.what()});
  }
}

} // namespace ReactNativeWebviewSpecs
```

## Step 5: Build and Test

```bash
# Build the Windows project
cd windows
msbuild ReactNativeWebview.sln

# Or use Visual Studio to open and build the solution
```

## Benefits of Using module-windows-setup

1. **Automation**: All setup steps are automated - no manual file creation or configuration
2. **Best Practices**: Generated files follow RNW coding standards and patterns
3. **Completeness**: Creates the entire Windows integration structure in one command
4. **Consistency**: Ensures consistent naming and structure across all Windows modules
5. **Error Prevention**: Validates environment and handles common setup issues
6. **Documentation**: Provides clear next steps and examples for implementation

## Alternative Usage Patterns

### Skip Dependency Upgrades
```bash
yarn react-native module-windows-setup --skip-deps
```

### Skip Build Verification
```bash
yarn react-native module-windows-setup --skip-build
```

### Verbose Logging
```bash
yarn react-native module-windows-setup --logging
```

### Minimal Setup (Skip upgrades and build)
```bash
yarn react-native module-windows-setup --skip-deps --skip-build
```

This command transforms any React Native community module into a fully Windows-compatible module with minimal effort and maximum reliability.