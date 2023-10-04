/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

const {getDefaultConfig, mergeConfig} = require('@react-native/metro-config');

const MetroConfig = require('@rnx-kit/metro-config');
const {MetroSerializer} = require('@rnx-kit/metro-serializer');
const {
  DuplicateDependencies,
} = require('@rnx-kit/metro-plugin-duplicates-checker');

function makeMetroConfig(customConfig = {}) {
  return mergeConfig(
    getDefaultConfig(customConfig?.projectRoot ?? __dirname),
    MetroConfig.makeMetroConfig(customConfig),
    {
      resolver: {
        blockList: MetroConfig.exclusionList([
          // This prevents "react-native run-windows" from hitting: EBUSY: resource
          // busy or locked for files produced by MSBuild
          /.*\/build\/.*/,
          /.*\/target\/.*/,
          /.*\.tlog/,
        ]),
      },
      serializer: {
        customSerializer: MetroSerializer([
          DuplicateDependencies({
            // Duplicate dependencies introduced by external code
            ignoredModules: [
              'react-is',
              'metro-runtime',
              '@react-native/normalize-colors',
            ],
          }),
        ]),
      },
      transformer: {
        // This fixes the 'missing-asset-registry-path` error (see https://github.com/microsoft/react-native-windows/issues/11437)
        assetRegistryPath: 'react-native/Libraries/Image/AssetRegistry',
      },
    },
  );
}

module.exports = {
  exclusionList: MetroConfig.exclusionList,
  makeMetroConfig: makeMetroConfig,
};
