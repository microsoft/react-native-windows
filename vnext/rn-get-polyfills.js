/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

'use strict';

module.exports = () => [
  require.resolve('./Libraries/polyfills/console.js'),
  require.resolve('./Libraries/polyfills/error-guard.js'),
  require.resolve('./Libraries/polyfills/Object.es7.js'),
];
