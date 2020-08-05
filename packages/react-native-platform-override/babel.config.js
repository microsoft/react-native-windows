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
        targets: {node: '12'},
      },
    ],
    '@babel/preset-typescript',
  ],
  plugins: [['@babel/plugin-transform-modules-commonjs', {noInterop: true}]],
};
