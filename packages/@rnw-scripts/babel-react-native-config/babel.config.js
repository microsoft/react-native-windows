/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @ts-check
 */

module.exports = () => ({
  presets: [
    'module:@react-native/babel-preset',
  ],
  plugins: [
    "@babel/plugin-transform-logical-assignment-operators", // Note, remove by 8/4 RN integrate or https://github.com/facebook/react-native/issues/31704 is fixed.
    "@babel/plugin-transform-nullish-coalescing-operator", // Note, remove by 8/4 RN integrate or https://github.com/facebook/react-native/issues/31704 is fixed.
    "babel-plugin-transform-flow-enums",
    ["@babel/plugin-transform-private-methods", { "loose": true }],
  ]
});
