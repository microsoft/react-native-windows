/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

module.exports = {
  presets: [
    [
      '@babel/preset-env',
      {
        targets: {node: '14'},
      },
    ],
    'module:metro-react-native-babel-preset',
    '@babel/preset-typescript',
  ],
};
