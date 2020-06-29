/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import * as envinfo from 'envinfo';

export async function getEnvironmentInfo() {
  return await envinfo.run({
    System: ['OS', 'CPU', 'Memory', 'Shell'],
    Binaries: ['Node', 'Yarn', 'npm', 'Watchman'],
    npmPackages: [
      'react',
      'react-native',
      'react-native-windows',
      '@react-native-community/cli',
      'react-native-cli',
    ],
    npmGlobalPackages: ['*react-native*'],
  });
}
