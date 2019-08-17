/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
'use strict';

const path = require('path');
const fs = require('fs');

/**
 * This cli config is needed for development purposes, e.g. for running
 * integration tests during local development or on CI services.
 */

let config = {
  extraNodeModules: {
    'react-native': __dirname,
    'react-native-windows': __dirname,
  },
  watchFolders: [
    __dirname,
    // Include hoisted modules
    path.resolve(__dirname, '..', 'node_modules'),
  ],
  getPolyfills: require('react-native/rn-get-polyfills'),
  resolver: {
    platforms: ['ios', 'android', 'windesktop', 'windows', 'web', 'macos'],
    providesModuleNodeModules: ['react-native', 'react-native-windows'],
    hasteImplModulePath: path.resolve(__dirname, 'jest/hasteImpl.js'),
  },
  projectRoot: __dirname,
};

// Check that we have built our JS files before running the bundler, otherwise we'll get a harder to diagnose "Unable to resolve module" error
if (
  !fs.existsSync(
    path.resolve(
      __dirname,
      'Libraries/Components/AccessibilityInfo/AccessibilityInfo.windows.js',
    ),
  )
) {
  throw new Error(
    '[31m\nThis package must be built before running the bundler.  Did you mean to run "[39m[33myarn build[39m[31m" first?[39m\n',
  );
}

module.exports = config;
