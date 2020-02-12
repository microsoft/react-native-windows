/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

'use strict';

const BatchedBridge = require('react-native/Libraries/BatchedBridge/BatchedBridge');

const warning = require('fbjs/lib/warning');
const invariant = require('invariant');

const LoggingTestModule = {
  logToConsole: function(str) {
    console.log(str);
  },
  logToConsoleAfterWait: function(str, timeout_ms) {
    setTimeout(function() {
      console.log(str);
    }, timeout_ms);
  },
  warning: function(str) {
    warning(false, str);
  },
  invariant: function(str) {
    invariant(false, str);
  },
  logErrorToConsole: function(str) {
    console.error(str);
  },
  throwError: function(str) {
    throw new Error(str);
  },
};

BatchedBridge.registerCallableModule('LoggingTestModule', LoggingTestModule);

module.exports = LoggingTestModule;
