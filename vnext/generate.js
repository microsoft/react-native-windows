/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @ts-check
 */

// This file provides a stable entry-point to call the generator for this
// version of react-native-windows.

try {
  module.exports = require('@react-native-windows/cli').generateWindows;
} catch (e) {
  console.error('Failure during generate ', e);
}
