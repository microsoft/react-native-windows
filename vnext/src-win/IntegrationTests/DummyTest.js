/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

const {TurboModuleRegistry} = require('react-native');
const TestModule = TurboModuleRegistry.get('TestModule');

if (!TestModule) {
  TestModule.markTestPassed(false, 'TestModule is not available');
}
TestModule.markTestPassed(true);
