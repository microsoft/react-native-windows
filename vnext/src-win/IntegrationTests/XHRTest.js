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

const xhr = new XMLHttpRequest();
xhr.onloadend = () => {
  const responseText = xhr.responseText || '';
  const passed =
    xhr.status === 200 && responseText === EXPECTED_CONTENT;
  TestModule.markTestPassed(passed);
};
xhr.onerror = () => {
  TestModule.markTestPassed(false);
};

xhr.open('GET', URL);
xhr.send();
