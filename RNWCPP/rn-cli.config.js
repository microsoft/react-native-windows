// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';
const path = require('path');

const fs = require('fs');

const platforms = ['ios', 'android', 'windesktop', 'uwp', 'web', 'macos'];

/**
 * This cli config is needed for development purposes, e.g. for running
 * integration tests during local development or on CI services.
 */

let config;

// In sdx-platform we use metro-resources to handle all the sym linking....
if (fs.existsSync(path.resolve(__dirname, '../../scripts/metro-resources.js'))) {
  const sdxHelpers = require('../../scripts/metro-resources');

  config = sdxHelpers.createConfig({
    getPlatforms() {
      return platforms;
    },
    projectRoot: __dirname,
    getPolyfills: require('../react-native/rn-get-polyfills'),
    roots: [__dirname],
  });

  config.resolver.extraNodeModules['@microsoft/react-native'] = path.resolve(__dirname, '../react-native');
  config.resolver.extraNodeModules['react-native/Libraries/Image/AssetRegistry'] = path.resolve(__dirname, '../react-native/Libraries/Image/AssetRegistry.js');
  config.resolver.providesModuleNodeModules = ['react-native', 'react-native-win'];
} else {
  const rootRnPath = path.resolve(require.resolve('@microsoft/react-native'), '../../..');

  config = {
    getPolyfills: require('@microsoft/react-native/rn-get-polyfills'),
    resolver: {
      extraNodeModules: {},
      platforms,
      providesModuleNodeModules: ['@microsoft\\\\react-native', 'react-native-win'],
    },
    projectRoot: __dirname,
  };

  config.resolver.extraNodeModules['@microsoft/react-native'] = rootRnPath;
  config.resolver.extraNodeModules['react-native'] = rootRnPath;
  config.resolver.extraNodeModules['react-native-win'] = __dirname;
  config.resolver.extraNodeModules['react-native/Libraries/Image/AssetRegistry'] = path.resolve(
    rootRnPath,
    'Libraries/Image/AssetRegistry.js'
  );
}

config.resolver.extraNodeModules['SnapshotViewIOS'] = path.resolve(__dirname, 'Libraries/RCTTest/SnapshotViewIOS');
config.resolver.hasteImplModulePath = path.resolve(__dirname, 'jest/hasteImpl.js');

module.exports = config;

