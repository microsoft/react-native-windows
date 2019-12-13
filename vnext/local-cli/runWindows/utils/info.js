/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
// @ts-check

const envinfo = require('envinfo');

async function getEnvironmentInfo() {
  return await envinfo.run({
    System: ['OS', 'CPU', 'Memory', 'Shell'],
    Binaries: ['Node', 'Yarn', 'npm', 'Watchman'],
    npmPackages: [
      'react',
      'react-native',
      'react-native-windows',
      '@react-native-community/cli',
      'rnpm-plugin-windows',
      'react-native-cli',
    ],
    npmGlobalPackages: ['*react-native*'],
  });
}

module.exports = {
  getEnvironmentInfo,
};
