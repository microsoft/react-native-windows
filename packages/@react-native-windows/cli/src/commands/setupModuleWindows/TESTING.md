# Testing the setup-module-windows Command

This guide explains how to test the `setup-module-windows` command locally with both `cpp-lib` and `cpp-app` templates.

## Prerequisites

1. **Build the CLI package locally**:
   ```bash
   cd /path/to/react-native-windows
   yarn install
   yarn build
   ```

2. **Ensure you have the required tools**:
   - Node.js (v20+)
   - Yarn package manager
   - Git
   - Windows SDK (for building C++ components)

## Test Scenarios

### Scenario 1: Test with cpp-lib template (Default)

Create a minimal test module to verify basic functionality:

```bash
# Create test directory
mkdir test-react-native-module
cd test-react-native-module

# Initialize package.json
npm init -y

# Update package.json with proper React Native module structure
cat > package.json << 'EOF'
{
  "name": "test-react-native-module",
  "version": "1.0.0",
  "description": "Test module for setup-module-windows",
  "main": "index.js",
  "repository": {
    "type": "git",
    "url": "https://github.com/test/test-react-native-module.git"
  },
  "keywords": ["react-native"],
  "author": "Test Author",
  "license": "MIT",
  "peerDependencies": {
    "react-native": "*"
  }
}
EOF

# Create TurboModule spec file (REQUIRED)
cat > NativeTestModule.ts << 'EOF'
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import type {TurboModule} from 'react-native/Libraries/TurboModule/RCTExport';
import {TurboModuleRegistry} from 'react-native';

export interface Spec extends TurboModule {
  getValue(key: string): Promise<string>;
  setValue(key: string, value: string): Promise<void>;
  getNumber(defaultValue: number): Promise<number>;
  getBooleanFlag(): Promise<boolean>;
}

export default TurboModuleRegistry.getEnforcing<Spec>('TestModule');
EOF

# Link to local CLI build
yarn add file:/absolute/path/to/react-native-windows/packages/@react-native-windows/cli

# Run the setup command with cpp-lib template (default)
yarn react-native setup-module-windows --logging
```

**Expected behavior**:
- Should find the spec file `NativeTestModule.ts`
- Should create `windows/TestModule/` directory structure
- Should generate header and cpp files with Hello World example
- Should include commented multiply function reference

### Scenario 2: Test with cpp-app template

```bash
# Using the same test module from Scenario 1
# Run with cpp-app template
yarn react-native setup-module-windows --template cpp-app --logging
```

**Expected behavior**:
- Should find the spec file `NativeTestModule.ts`
- Should create application project structure in `windows/`
- Should include XAML files for Windows app
- Should generate module files in appropriate subdirectory

### Scenario 3: Test error handling - No spec file

```bash
# Create test directory without spec file
mkdir test-no-spec-module
cd test-no-spec-module

# Initialize package.json
npm init -y

# Link to local CLI build
yarn add file:/absolute/path/to/react-native-windows/packages/@react-native-windows/cli

# Run the setup command (should fail)
yarn react-native setup-module-windows --logging
```

**Expected behavior**:
- Should fail with error: "Create Spec File - TurboModule spec file not found. Please create a TurboModule spec file before running setup-module-windows."
- Should not create any Windows files
- Should exit with non-zero code

### Scenario 4: Test with real community module

Test with a real module like `react-native-webview`:

```bash
# Clone the repository
git clone https://github.com/react-native-webview/react-native-webview.git
cd react-native-webview

# Create a test spec file (since real module may not have one)
cat > NativeReactNativeWebview.ts << 'EOF'
import type {TurboModule} from 'react-native/Libraries/TurboModule/RCTExport';
import {TurboModuleRegistry} from 'react-native';

export interface Spec extends TurboModule {
  goBack(): void;
  goForward(): void;
  reload(): void;
  postMessage(message: string): void;
}

export default TurboModuleRegistry.getEnforcing<Spec>('ReactNativeWebview');
EOF

# Link to your local CLI build
yarn add file:/absolute/path/to/react-native-windows/packages/@react-native-windows/cli

# Test with cpp-lib template
yarn react-native setup-module-windows --template cpp-lib --logging

# Clean up and test with cpp-app template
rm -rf windows codegen
yarn react-native setup-module-windows --template cpp-app --logging
```

## Validation Steps

After running the command, verify:

1. **Generated files exist**:
   ```bash
   ls -la Native*.ts                    # TypeScript spec file (should exist before)
   ls -la windows/                      # Windows directory
   ls -la windows/*/*.h windows/*/*.cpp # C++ implementation files
   ```

2. **Package.json updated**:
   ```bash
   cat package.json | grep -A 10 "codegenConfig"
   ```

3. **Template-specific files**:
   
   For **cpp-lib**:
   ```bash
   ls -la windows/ModuleName/           # Module subdirectory
   ls -la windows/*.sln                 # Solution file
   ```
   
   For **cpp-app**:
   ```bash
   ls -la windows/*.xaml                # XAML files
   ls -la windows/ModuleName/           # Module subdirectory
   ls -la windows/*.sln                 # Solution file
   ```

4. **Generated code content**:
   - Check header file includes Hello World function and commented multiply function
   - Check cpp file has working Hello World implementation
   - Verify namespace and class names match spec

5. **Project builds** (optional):
   ```bash
   cd windows && msbuild *.sln
   ```

## Command Line Options Testing

Test different combinations of options:

```bash
# Basic setup with default template
yarn react-native setup-module-windows

# With verbose logging
yarn react-native setup-module-windows --logging

# Different templates
yarn react-native setup-module-windows --template cpp-lib
yarn react-native setup-module-windows --template cpp-app

# Skip options
yarn react-native setup-module-windows --skip-deps
yarn react-native setup-module-windows --skip-build

# Combined options
yarn react-native setup-module-windows --template cpp-app --logging --skip-deps
```

## Expected File Structure

### For cpp-lib template (default):
```
test-module/
├── package.json (updated)
├── NativeTestModule.ts (spec file)
├── windows/
│   ├── TestModule/
│   │   ├── TestModule.h
│   │   └── TestModule.cpp
│   └── TestModule.sln
└── codegen/
    └── NativeTestModuleSpec.g.h
```

### For cpp-app template:
```
test-module/
├── package.json (updated)
├── NativeTestModule.ts (spec file)
├── windows/
│   ├── App.xaml
│   ├── MainWindow.xaml
│   ├── TestModule/
│   │   ├── TestModule.h
│   │   └── TestModule.cpp
│   └── TestModule.sln
└── codegen/
    └── NativeTestModuleSpec.g.h
```

## Generated Code Verification

### Header File Should Contain:
```cpp
// Reference function for demonstration (from cpp-lib template)
// double multiply(double a, double b) noexcept { return a * b; }

// Hello World example to verify module functionality
REACT_METHOD(sayHello)
void sayHello(std::string name, std::function<void(std::string)> const & callback) noexcept;
```

### Implementation File Should Contain:
```cpp
void TestModule::sayHello(std::string name, std::function<void(std::string)> const & callback) noexcept {
  std::string result = "Hello " + name + "! Module is working.";
  callback(result);
}
```

## Troubleshooting

### Command not found
If you get "command not found":
```bash
# Verify the CLI package is linked
ls node_modules/@react-native-windows/cli

# Try using npx directly
npx @react-native-windows/cli setup-module-windows
```

### Template validation
Test that invalid templates are rejected:
```bash
# Should fail with error
yarn react-native setup-module-windows --template invalid-template
```

### Build failures
If builds fail:
- Use `--skip-build` to focus on file generation
- Check Windows SDK installation
- Verify project structure matches expectations

## Performance Testing

Test with timing to ensure reasonable performance:
```bash
time yarn react-native setup-module-windows --logging
```

Expected completion time should be under 2 minutes for most modules.

## Contributing

When making changes to the command:

1. Test with both template options (`cpp-lib` and `cpp-app`)
2. Verify error handling with missing spec files
3. Check generated code follows RNW conventions
4. Test with multiple real-world modules
5. Ensure command-line options work correctly
6. Verify telemetry integration

This comprehensive testing ensures the `setup-module-windows` command works reliably across different module structures and template types.