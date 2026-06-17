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
        targets: {node: '22'},
      },
    ],
    '@babel/preset-typescript',
  ],
  plugins: [
    ["@babel/plugin-transform-private-methods", { "loose": true }],
    ["@babel/plugin-transform-private-property-in-object", { "loose": true }]
  ]
};
