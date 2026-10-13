/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

const {mergeConfig} = require('@react-native/metro-config');

const MetroConfig = require('@rnx-kit/metro-config');
const {MetroSerializer} = require('@rnx-kit/metro-serializer');
const {
  DuplicateDependencies,
} = require('@rnx-kit/metro-plugin-duplicates-checker');

// eslint-disable-next-line @react-native/no-deep-imports
const reactNativePackage = require('react-native/package.json');

function resolveReactNativeExport(moduleName) {
  const prefix = 'react-native/';
  if (!moduleName.startsWith(prefix)) {
    return moduleName;
  }

  const subpath = moduleName.slice(prefix.length);
  let target = reactNativePackage.exports?.[`./${subpath}`];
  while (target && typeof target === 'object') {
    target =
      target['react-native'] ??
      target.default ??
      target.require ??
      target.import ??
      null;
  }

  return typeof target === 'string'
    ? `${prefix}${target.replace(/^[.][/]/, '')}`
    : moduleName;
}

function makeMetroConfig(customConfig = {}) {
  if (customConfig.unstable_allowAssetsOutsideProjectRoot === undefined)
    customConfig.unstable_allowAssetsOutsideProjectRoot = true;

  const metroConfig = MetroConfig.makeMetroConfig(customConfig);
  const resolveRequest = metroConfig.resolver.resolveRequest;
  metroConfig.resolver.resolveRequest = (context, moduleName, platform) =>
    resolveRequest(
      context,
      resolveReactNativeExport(moduleName),
      platform,
    );

  return mergeConfig(metroConfig, {
    resolver: {
      enableGlobalPackages: true,
      blockList: MetroConfig.exclusionList([
        // This prevents "npx @react-native-community/cli run-windows" from hitting: EBUSY: resource
        // busy or locked for files produced by MSBuild
        /.*\/build\/.*/,
        /.*\/target\/.*/,
        /.*\/\.vs\/.*/,
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
  });
}

module.exports = {
  exclusionList: MetroConfig.exclusionList,
  makeMetroConfig: makeMetroConfig,
};
