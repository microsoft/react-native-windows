# Testing the module-windows-setup Command

This guide explains how to test the `module-windows-setup` command locally with real React Native community modules.

## Prerequisites

1. **Build the CLI package locally**:
   ```bash
   cd /path/to/react-native-windows
   yarn install
   yarn build
   ```

2. **Ensure you have the required tools**:
   - Node.js (v16+)
   - Yarn package manager
   - Git
   - Windows SDK (for building C++ components)

## Test Scenarios

### Scenario 1: Test with react-native-webview

This module has extensive iOS and Android implementations that the command can analyze.

```bash
# Clone the repository
git clone https://github.com/react-native-webview/react-native-webview.git
cd react-native-webview

# Link to your local CLI build
yarn add file:/absolute/path/to/react-native-windows/packages/@react-native-windows/cli

# Run the setup command with verbose logging
yarn react-native module-windows-setup --logging
```

**Expected behavior**:
- Should discover APIs from Android Java files (e.g., `goBack()`, `reload()`, `postMessage()`)
- Should discover APIs from iOS Objective-C files  
- Should create proper TypeScript spec with discovered methods
- Should generate C++ stubs with matching signatures

### Scenario 2: Test with notifee

This module has both native implementations and good documentation.

```bash
# Clone the repository
git clone https://github.com/invertase/notifee.git
cd notifee

# Link to your local CLI build  
yarn add file:/absolute/path/to/react-native-windows/packages/@react-native-windows/cli

# Run the setup command
yarn react-native module-windows-setup --logging
```

**Expected behavior**:
- Should discover notification-related APIs
- Should analyze the main TypeScript files for exported functions
- Should generate appropriate Windows-specific implementations

### Scenario 3: Test with a minimal custom module

Create a test module from scratch to verify all functionality:

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
  "description": "Test module for module-windows-setup",
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

# Create main entry point with TypeScript APIs
cat > index.ts << 'EOF'
export interface TestModuleType {
  getValue(key: string): Promise<string>;
  setValue(key: string, value: string): Promise<void>;
  getNumber(defaultValue: number): Promise<number>;
  getBooleanFlag(): Promise<boolean>;
  performAction(config: object): Promise<any>;
}

declare const TestModule: TestModuleType;
export default TestModule;
EOF

# Create Android implementation for API discovery
mkdir -p android/src/main/java/com/testmodule
cat > android/src/main/java/com/testmodule/TestModule.java << 'EOF'
package com.testmodule;

import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.bridge.Promise;

public class TestModule extends ReactContextBaseJavaModule {
    TestModule(ReactApplicationContext context) {
        super(context);
    }

    @Override
    public String getName() {
        return "TestModule";
    }

    @ReactMethod
    public void getValue(String key, Promise promise) {
        promise.resolve("test-value");
    }

    @ReactMethod  
    public void setValue(String key, String value, Promise promise) {
        promise.resolve(null);
    }

    @ReactMethod
    public void getNumber(Double defaultValue, Promise promise) {
        promise.resolve(42.0);
    }

    @ReactMethod
    public void getBooleanFlag(Promise promise) {
        promise.resolve(true);
    }

    @ReactMethod
    public void performAction(ReadableMap config, Promise promise) {
        promise.resolve(null);
    }
}
EOF

# Create iOS implementation for API discovery
mkdir -p ios
cat > ios/TestModule.m << 'EOF'
#import "TestModule.h"

@implementation TestModule

RCT_EXPORT_MODULE()

RCT_EXPORT_METHOD(getValue:(NSString *)key
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
    resolve(@"test-value");
}

RCT_EXPORT_METHOD(setValue:(NSString *)key
                  value:(NSString *)value
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
    resolve(nil);
}

RCT_EXPORT_METHOD(getNumber:(NSNumber *)defaultValue
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
    resolve(@42);
}

RCT_EXPORT_METHOD(getBooleanFlag:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
    resolve(@YES);
}

RCT_EXPORT_METHOD(performAction:(NSDictionary *)config
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
    resolve(nil);
}

@end
EOF

# Create README with documented API
cat > README.md << 'EOF'
# Test React Native Module

## API

```javascript
import TestModule from 'test-react-native-module';

// Get a string value
const value = await TestModule.getValue('key');

// Set a string value  
await TestModule.setValue('key', 'value');

// Get a number
const number = await TestModule.getNumber(10);

// Get a boolean flag
const flag = await TestModule.getBooleanFlag();

// Perform an action
await TestModule.performAction({option: 'value'});
```
EOF

# Link to local CLI build
yarn add file:/absolute/path/to/react-native-windows/packages/@react-native-windows/cli

# Run the setup command
yarn react-native module-windows-setup --logging
```

**Expected behavior**:
- Should discover all 5 methods from TypeScript, Android, and iOS sources
- Should create a comprehensive spec file with proper types
- Should generate C++ header and implementation files with correct signatures
- Should map types correctly (string → std::string, number → double, etc.)

## Validation Steps

After running the command, verify:

1. **Generated files exist**:
   ```bash
   ls -la Native*.ts                    # TypeScript spec file
   ls -la windows/*.h windows/*.cpp     # C++ implementation files
   ```

2. **Package.json updated**:
   ```bash
   cat package.json | grep -A 10 "codegenConfig"
   ```

3. **API discovery worked**:
   - Check the TypeScript spec file for discovered methods
   - Verify C++ files have matching method signatures
   - Confirm type mappings are correct

4. **Project builds** (optional):
   ```bash
   yarn react-native module-windows-setup --skip-build
   # Then manually build to test
   cd windows && msbuild *.sln
   ```

## Command Line Options

Test different options to verify functionality:

```bash
# Basic setup
yarn react-native module-windows-setup

# With verbose logging
yarn react-native module-windows-setup --logging

# Skip dependency upgrades (faster for testing)
yarn react-native module-windows-setup --skip-deps

# Skip build verification
yarn react-native module-windows-setup --skip-build

# Minimal setup (fastest for iteration)
yarn react-native module-windows-setup --skip-deps --skip-build --logging
```

## Troubleshooting

### Command not found
If you get "command not found":
```bash
# Verify the CLI package is linked
ls node_modules/@react-native-windows/cli

# Try using npx directly
npx @react-native-windows/cli module-windows-setup
```

### No APIs discovered
If no APIs are discovered:
- Check that files exist in expected locations (`android/`, `ios/`, main entry point)
- Use `--logging` to see what files are being analyzed
- Verify file patterns match (`.java`, `.kt`, `.m`, `.mm` files)

### Build failures
If builds fail:
- Use `--skip-build` to focus on file generation
- Check Windows SDK installation
- Verify project structure matches expectations

## Contributing

When making changes to the command:

1. Test with multiple real-world modules
2. Verify API discovery from all sources (JS/TS, Android, iOS, README)
3. Check type mapping accuracy
4. Ensure generated files follow RNW conventions
5. Test error handling with malformed inputs

This comprehensive testing ensures the `module-windows-setup` command works reliably across different module structures and API patterns.