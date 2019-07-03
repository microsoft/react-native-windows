/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
'use strict';

const path = require('path');

const fs = require('fs');

const utils = require('./Scripts/utils');

const platforms = ['ios', 'android', 'windesktop', 'uwp', 'web', 'macos'];

/**
 * This cli config is needed for development purposes, e.g. for running
 * integration tests during local development or on CI services.
 */

let config;

// In sdx-platform we use metro-resources to handle all the sym linking....
if (
  fs.existsSync(path.resolve(__dirname, '../../scripts/metro-resources.js'))
) {
  const sdxHelpers = require('../../scripts/metro-resources');

  config = sdxHelpers.createConfig({
    getPlatforms() {
      return platforms;
    },
    projectRoot: __dirname,
    getPolyfills: require('../react-native/rn-get-polyfills'),
    roots: [__dirname],
  });

  config.resolver.extraNodeModules['react-native'] = path.resolve(
    __dirname,
    '../react-native',
  );
  config.resolver.extraNodeModules[
    'react-native/Libraries/Image/AssetRegistry'
  ] = path.resolve(
    __dirname,
    '../react-native/Libraries/Image/AssetRegistry.js',
  );
  config.resolver.providesModuleNodeModules = [
    'react-native',
    'react-native-windows',
  ];
} else {
  const rootRnPath = path.resolve(require.resolve('react-native'), '../../..');

  config = {
    extraNodeModules: {
      'react-native': rootRnPath,
      'react-native-windows': __dirname,
    },

    getPolyfills: require('react-native/rn-get-polyfills'),
    resolver: {
      platforms,
      providesModuleNodeModules: ['react-native', 'react-native-windows'],
    },
    projectRoot:
      utils.getDirectoryNameOfFileAbove(__dirname, 'app.json') || __dirname,
  };
}

config.extraNodeModules.SnapshotViewIOS = path.resolve(
  __dirname,
  'Libraries/RCTTest/SnapshotViewIOS',
);
config.resolver.hasteImplModulePath = path.resolve(
  __dirname,
  'jest/hasteImpl.js',
);

// Check that we have built our JS files before running the bundler, otherwise we'll get a harder to diagnose "Unable to resolve module" error
if (
  !fs.existsSync(
    path.resolve(
      __dirname,
      'lib/Libraries/Components/AccessibilityInfo/AccessibilityInfo.uwp.js',
    ),
  )
) {
  throw new Error(
    '[31m\nThis package must be built before running the bundler.  Did you mean to run "[39m[33myarn build[39m[31m" first?[39m\n',
  );
}

module.exports = config;
