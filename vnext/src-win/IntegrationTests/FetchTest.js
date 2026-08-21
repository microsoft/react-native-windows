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

const URL = 'http://localhost:5555/static/sample.txt';
const EXPECTED_CONTENT = 'Sample Static Text File';

fetch(URL)
  .then(response => response.text())
  .then(text => {
    const passed = text === EXPECTED_CONTENT;
    TestModule.markTestPassed(passed);
  })
  .catch(() => {
    TestModule.markTestPassed(false);
  });
