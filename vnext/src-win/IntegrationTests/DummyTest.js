/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

// Don't require 'react-native' — it triggers initialization that may fail
// in a headless test environment (missing AppState, etc.).
// Access the TurboModule binding directly via the bridgeless global proxy.
const TestModule = global.nativeModuleProxy?.TestModule;
if (TestModule) {
  TestModule.markTestPassed(true);
} else {
  // Fallback: try the non-bridgeless proxy
  const proxy = global.__turboModuleProxy;
  if (proxy) {
    const mod = proxy('TestModule');
    if (mod) {
      mod.markTestPassed(true);
    }
  }
}
