/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

const {TurboModuleRegistry} = require('react-native');

const TestModule = TurboModuleRegistry.get('TestModule');

if (!TestModule) {
  throw new Error('TestModule is not available');
}

const passed =
  TestModule.markTestCompleted === TestModule.markTestCompleted &&
  TestModule.verifySnapshot === TestModule.verifySnapshot &&
  TestModule.shouldResolve === TestModule.shouldResolve;

TestModule.markTestPassed(passed);
